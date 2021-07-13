/*
 * vsnprintf.c
 *
 *  Created on: Jul 3, 2021
 *      Author: Paulo Almeida
 */
#include "kernel/lib/vsnprintf.h"
#include "kernel/lib/ctype.h"
#include "kernel/lib/string.h"
#include "kernel/compiler/macro.h"

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
    UNKNOWN_LENGTH, LONG_LENGTH, LONG_LONG_LENGTH, ERROR_LENGTH
};

#define PRINTK_DECD_ERROR_MSG "<FORMAT_DECODE_ERROR>"
#define PRINTK_PROC_ERROR_MSG "<FORMAT_PROCESS_ERROR>"

static size_t format_decode(const char *fmt, print_fmt_spec *spec);
static size_t process_format(char *buf, size_t buf_size, print_fmt_spec *spec, va_list args);
static size_t read_precision(const char *fmt, print_fmt_spec *spec);
static size_t read_length(const char *fmt, print_fmt_spec *spec);
static size_t read_type(const char *fmt, print_fmt_spec *spec);
static bool is_length(const char c);
static bool is_type(const char c);

size_t vsnprintf(char *buf, size_t buf_size, const char *fmt, va_list args) {

    /*
     * Scratch pad:
     * 	%llu -> multiple letters (length)
     * 	%.5d -> Precision/Width
     * 	%.15d -> Precision/Width with multiple numbers
     * 	%x -> base must be inferred
     *
     * 	I have to be careful when it comes to not exceeding the buf size for security reasons
     */

    size_t read_chars = 0;

    for (size_t i = 0; fmt[i] != '\0'; i++) {
        char c = fmt[i];
        if (c == '%') {
            // hold meta-data about format string spec.
            print_fmt_spec spec = { 0 };

            // extract info about what needs to be printed
            int read = format_decode(fmt + i, &spec) - 1;

            // check if we haven't had trouble parsing the string format
            if (spec.type != ERROR_TYPE && spec.length != ERROR_LENGTH) {
                i += read;
                // print the actual stuff
                read_chars += process_format(buf + read_chars, buf_size - read_chars, &spec, args);
            } else {
                // something went wrong... surface the symptom up
                size_t err_msg_len = strlen(PRINTK_DECD_ERROR_MSG);
                memcpy(buf + read_chars, PRINTK_DECD_ERROR_MSG, MIN(buf_size - read_chars, err_msg_len + 1));
                read_chars += err_msg_len;
                // fail-fast
                break;
            }

            // check if this isn't time to go home - truncate content if needed
            if (read_chars == buf_size)
                break;
        } else if (c == '\t') {
            // yes, tab is 4 spaces... if you don't like you can write your own OS ;)
            for(size_t i =0; i < 4; i++){
                buf[read_chars++] = ' ';
            }
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

    error:
    spec->length = ERROR_LENGTH;
    spec->type = ERROR_TYPE;

    return 0;
}

static size_t process_format(char *buf, size_t buf_size, print_fmt_spec *spec, va_list args) {
    // watch out for buf_size
    char tmp_buf[1024];
    size_t tmp_buf_idx = 0;
    bool is_negative = false;
    memset(tmp_buf, '\0', ARR_SIZE(tmp_buf));

    if (spec->base == 0) {
        // this can only be string or char
        if (spec->type == CHAR_TYPE) {
            /* 'char' is promoted to 'int' when passed through '...' */
            tmp_buf[tmp_buf_idx++] = (char) va_arg(args, int);
        } else if (spec->type == STRING_TYPE) {
            const char *va_arr;
            size_t va_arr_size;

            va_arr = va_arg(args, const char*);
            va_arr_size = strlen(va_arr);
            tmp_buf_idx = MIN(ARR_SIZE(tmp_buf), va_arr_size);

            memcpy(tmp_buf, va_arr, tmp_buf_idx);
        } else {
            // LITERAL_TYPE
            tmp_buf[tmp_buf_idx++] = '%';
        }
    } else if (spec->type == UNSIGNED_INT_DEC_TYPE || spec->type == UNSIGNED_INT_OCT_TYPE
            || spec->type == UNSIGNED_INT_HEX_TYPE) {

        // this can be %u, %o or %x
        unsigned long long num;
        if (spec->length == UNKNOWN_LENGTH) {
            num = va_arg(args, unsigned int);
        } else if (spec->length == LONG_LENGTH) {
            num = va_arg(args, unsigned long int);
        } else if (spec->length == LONG_LONG_LENGTH) {
            num = va_arg(args, unsigned long long);
        }
        ulltoa(num, tmp_buf, spec->base);
        tmp_buf_idx = strlen(tmp_buf);

    } else if (spec->type == SIGNED_INT_TYPE) {

        // signed integers get messy if you don't cast it to the right VAR type - you know ... 2's complement
        long long int num;
        if (spec->length == UNKNOWN_LENGTH) {
            num = va_arg(args, int);
        } else if (spec->length == LONG_LENGTH) {
            num = va_arg(args, long int);
        } else if (spec->length == LONG_LONG_LENGTH) {
            num = va_arg(args, long long int);
        }
        if (num < 0)
            is_negative = true;
        lltoa(num, tmp_buf, spec->base);
        tmp_buf_idx = strlen(tmp_buf);

    } else {

        // this is an error
        size_t err_msg_len = strlen(PRINTK_PROC_ERROR_MSG);
        memcpy(tmp_buf, PRINTK_PROC_ERROR_MSG, MIN(buf_size, err_msg_len + 1));
        tmp_buf_idx = err_msg_len;
    }

    // check for precision here so we only copy what we are meant to.
    if (spec->precision > 0) {
        size_t left_pointer;
        switch (spec->type) {
        case UNSIGNED_INT_DEC_TYPE:
        case UNSIGNED_INT_HEX_TYPE:
        case UNSIGNED_INT_OCT_TYPE:
        case SIGNED_INT_TYPE:
            if (spec->precision > tmp_buf_idx) {

                left_pointer = spec->precision - tmp_buf_idx;

                if (is_negative) {
                    *(buf++) = '-';
                    left_pointer--;
                    tmp_buf[0] = '0';
                }

                while (left_pointer-- > 0) {
                    *(buf++) = '0';
                }

                tmp_buf_idx = spec->precision;
            }
            break;
        case STRING_TYPE:
            // truncate string
            if (spec->precision < tmp_buf_idx) {
                memset(tmp_buf + spec->precision, '\0', spec->precision);
                tmp_buf_idx = spec->precision;
            }

            break;
        }
    }

    memcpy(buf, tmp_buf, MIN(buf_size, tmp_buf_idx));

    return tmp_buf_idx;
}

static size_t read_precision(const char *fmt, print_fmt_spec *spec) {
    size_t read_chars = 0;
    char c = *fmt;

    do {
        spec->precision = spec->precision * 10 + ((unsigned) c - '0');
        read_chars++;
    } while (isdigit((c = *(fmt + read_chars))));

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
        }

        read_chars++;
    } while (is_length((c = *(fmt + read_chars))));

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
            spec->base = 10;
        } else if (c == 'u') {
            spec->type = UNSIGNED_INT_DEC_TYPE;
            spec->base = 10;
        } else if (c == 'x') {
            spec->type = UNSIGNED_INT_HEX_TYPE;
            spec->base = 16;
        } else if (c == 'o') {
            spec->type = UNSIGNED_INT_OCT_TYPE;
            spec->base = 8;
        } else if (c == 'c') {
            spec->type = CHAR_TYPE;
            spec->base = 0;
        } else if (c == 's') {
            spec->type = STRING_TYPE;
            spec->base = 0;
        }

        read_chars++;
    } while (is_type((c = *(fmt + read_chars))));

    return read_chars;
}

static bool is_length(const char c) {
    return c == 'l';
}

static bool is_type(const char c) {
    return c == 'd' || c == 'i' || c == 'u' || c == 'x' || c == 'o' || c == 'c' || c == 's';
}

