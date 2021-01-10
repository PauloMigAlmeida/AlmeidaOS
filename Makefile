#----------------------------------------------------------------------------
# AlmeidaOS root makefile
#
# Makefile for all kernel and boot loader targets.
#----------------------------------------------------------------------------

DIR_ROOT := .

include $(DIR_ROOT)/scripts/config.mk


#----------------------------------------------------------------------------
# Build targets
#----------------------------------------------------------------------------

default: boot

all: boot 

.PHONY: boot
boot: 
	@$(MAKE) $(MAKE_FLAGS) --directory=$(DIR_BOOT)
	@# Help: Build the bootloader

.PHONY: qemu-debug-boot
qemu-debug-boot: 
	@$(QEMU) -gdb tcp::8864 -drive format=raw,file=$(DIR_BUILD)/boot/boot.bin -S
	@# Help: Runs QEMU in debug mode so that we can debug the bootloader

.PHONY: gdb-debug-boot
gdb-debug-boot: 
	@$(GDB) -x scripts/gdb/debug_commands.txt
	@# Help: Runs GDB with some personal preferences to debug the bootloader

.PHONY: test
test: 
	@$(QEMU) -drive format=raw,file=$(DIR_BUILD)/boot/boot.bin
	@# Help: Runs QEMU without debugging settings

.PHONY: clean
clean: 
	@rm -rf $(DIR_BUILD)
	@echo "[clean] Generated files deleted"
	@# Help: Clean all generated files

MAKEOVERRIDES =
help:
	@printf "%-20s %s\n" "Target" "Description"
	@printf "%-20s %s\n" "------" "-----------"
	@make -pqR : 2>/dev/null \
        	| awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' \
        	| sort \
        	| egrep -v -e '^[^[:alnum:]]' -e '^$@$$' \
        	| xargs -I _ sh -c 'printf "%-20s " _; make _ -nB | (grep -i "^# Help:" || echo "") | tail -1 | sed "s/^# Help: //g"'
