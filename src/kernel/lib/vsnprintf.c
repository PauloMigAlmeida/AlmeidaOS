/*
 * vsnprintf.c
 *
 *  Created on: Jul 3, 2021
 *      Author: Paulo Almeida
 */
#include "kernel/lib/vsnprintf.h"
#include "kernel/lib/ctype.h"
#include "kernel/lib/string.h"

/*
 * Notes to myself:
 *
 * The syntax for the format placeholder spec is
 * 		%[parameter][flags][width][.precision][length]type
 *
 * I don't intend to implement a fully-fledged version of that for many reasons,
 * instead, I will add new functionality here on-demand. For now the spec is
 *
 * 		%[.precision][length]type
 *
 * This should be enough for 99% of the cases I envision using for now.
 *
 * More: https://en.wikipedia.org/wiki/Printf_format_string
 */

struct print_fmt_spec {
	uint8_t type :8;
	uint8_t length :8;
	uint8_t base :8;
	uint8_t precision :8;
} __packed;
typedef struct print_fmt_spec print_fmt_spec;

enum printf_type {
	/* literal character such as % */
	UNKNOWN_TYPE,
	LITERAL_TYPE,
	SIGNED_INT_TYPE,
	UNSIGNED_INT_DEC_TYPE,
	UNSIGNED_INT_HEX_TYPE,
	UNSIGNED_INT_OCT_TYPE,
	CHAR_TYPE,
	STRING_TYPE,
	ERROR_TYPE
};

enum printf_length {
	UNKNOWN_LENGTH, LONG_LENGTH, LONG_LONG_LENGTH, SIZE_T_LENGTH, ERROR_LENGTH
};

#define PRINTF_ERROR_MSG "<PRINTK_FORMAT_ERROR>"

static size_t format_decode(const char *fmt, print_fmt_spec *spec);
static size_t process_format(char *buf, size_t buf_size, print_fmt_spec *spec,
		va_list args);
static size_t read_precision(const char *fmt, print_fmt_spec *spec);
static size_t read_length(const char *fmt, print_fmt_spec *spec);
static size_t read_type(const char *fmt, print_fmt_spec *spec);
static bool is_length(const char c);
static bool is_type(const char c);

size_t vsnprintf(char *buf, size_t buf_size, const char *fmt, va_list args) {
	// reject out of range sizes to avoid buffer overflow
	if (buf_size > (~(unsigned int) 0)) {
		return 0;
	}

	/*
	 * Scratch pad:
	 * 	%llu -> multiple letters (length)
	 * 	%.5d -> Precision/Width
	 * 	%.15d -> Precision/Width with multiple numbers
	 * 	%x -> base must be inferred
	 *
	 * 	I have to be careful when it comes to not exceed the buf size for sec
	 * 	reasons
	 */

	size_t read_chars = 0;
	print_fmt_spec spec = { 0 };

	for (size_t i = 0; fmt[i] != '\0'; i++) {
		char c = fmt[i];
		if (c == '%') {
			// extract info about what needs to be printed
			int read = format_decode(fmt + i, &spec) - 1;

			// check if we haven't had trouble parsing the string format
			if (spec.type != ERROR_TYPE && spec.length != ERROR_LENGTH) {
				i += read;
				// print the actual stuff
				read_chars += process_format(buf + read_chars, buf_size, &spec,
						args);
			} else {
				// something went wrong... surface the symptom up
				int err_msg_len = strlen(PRINTF_ERROR_MSG);
				memcpy(buf + read_chars, PRINTF_ERROR_MSG, err_msg_len + 1);
				read_chars += err_msg_len;
				// go home
				break;
			}

			// check if this isn't time to go home - truncate content if needed
			if (read_chars == (buf_size - 1))
				break;
		} else {
			buf[read_chars++] = c;
		}
	}

	return read_chars;
}

static size_t format_decode(const char *fmt, print_fmt_spec *spec) {
	size_t i = 0;
	bool found_dot = false;
	bool first_perc = false;
	// %[.precision][length]type
	do {
		char c = fmt[i];
		if (c == '%') {
			if (!first_perc) {
				first_perc = true;
				i++;
			} else {
				spec->type = LITERAL_TYPE;
				i++;
				break;
			}
		} else if (!found_dot && c == '.') {
			found_dot = true;
			i++;
		} else if (found_dot && isdigit(c)) {
			// read precision
			i += read_precision(fmt + i, spec);
			found_dot = false;
		} else if (found_dot && !isdigit(c)) {
			// this is a mistake - abort
			found_dot = false;
			goto error;
		} else if (!found_dot && is_length(c)) {
			//read length
			i += read_length(fmt + i, spec);
		} else if (!found_dot && is_type(c)) {
			//this can only be the type (unless I missed something in my logic)
			i += read_type(fmt + i, spec);
			break;
		} else {
			/* this case will only happen if we get a different char before
			 * reading a spec type - which is an error tbh */
			goto error;
		}

	} while (true);

	return i;

	error: spec->length = ERROR_LENGTH;
	spec->type = ERROR_TYPE;
	return 0;
}

static size_t process_format(char *buf, size_t buf_size, print_fmt_spec *spec,
		va_list args) {
	return 0;
}

static size_t read_precision(const char *fmt, print_fmt_spec *spec) {
	size_t read_chars = 0;
	char c = *fmt;

	do {
		spec->precision = spec->precision * 10 + ((unsigned) c - '0');
		read_chars++;
	} while (isdigit((c = *(fmt + read_chars + 1))));

	return read_chars;
}

static size_t read_length(const char *fmt, print_fmt_spec *spec) {
	size_t read_chars = 0;
	char c = *fmt;

	do {

		if (c == 'l') {
			if (spec->length == UNKNOWN_LENGTH)
				spec->length = LONG_LENGTH;
			else if (spec->length == LONG_LENGTH)
				spec->length = LONG_LONG_LENGTH;
			else
				spec->length = ERROR_LENGTH;

		} else if (c == 'z') {
			if (spec->length == UNKNOWN_LENGTH)
				spec->length = SIZE_T_LENGTH;
			else
				spec->length = ERROR_LENGTH;
		}

		read_chars++;
	} while (is_length((c = *(fmt + read_chars + 1))));

	return read_chars;
}

static size_t read_type(const char *fmt, print_fmt_spec *spec) {
	size_t read_chars = 0;
	char c = *fmt;

	do {

		if (spec->type != UNKNOWN_TYPE) {
			spec->type = ERROR_TYPE;
		} else if (c == 'd' || c == 'i') {
			spec->type = SIGNED_INT_TYPE;
		} else if (c == 'u') {
			spec->type = UNSIGNED_INT_DEC_TYPE;
		} else if (c == 'x') {
			spec->type = UNSIGNED_INT_HEX_TYPE;
		} else if (c == 'o') {
			spec->type = UNSIGNED_INT_OCT_TYPE;
		} else if (c == 'c') {
			spec->type = CHAR_TYPE;
		} else if (c == 's') {
			spec->type = STRING_TYPE;
		}

		read_chars++;
	} while (is_type((c = *(fmt + read_chars + 1))));

	return read_chars;
}

static bool is_length(const char c) {
	return c == 'l' || c == 'z';
}

static bool is_type(const char c) {
	return c == 'd' || c == 'i' || c == 'u' || c == 'x' || c == 'o' || c == 'c'
			|| c == 's';
}

