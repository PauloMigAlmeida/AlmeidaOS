#----------------------------------------------------------------------------
# AlmeidaOS root makefile
#
# Makefile for all kernel and boot loader targets.
#----------------------------------------------------------------------------

DIR_ROOT := $(CURDIR)

include $(DIR_ROOT)/scripts/config.mk
include $(DIR_ROOT)/docker/config.mk


#----------------------------------------------------------------------------
# Build targets
#----------------------------------------------------------------------------

default: boot raw-disk

all: boot raw-disk

.PHONY: boot
boot:
	@$(MAKE) $(MAKE_FLAGS) --directory=$(DIR_BOOT)
	@# Help: Build the bootloader

.PHONY: raw-disk
raw-disk:
	@echo "[raw-disk] Creating Raw disk image containing the AlmeidaOS"
	@$(DIR_SCRIPTS)/raw_disk.sh
	@# Help: Creates a Raw disk image containing the AlmeidaOS

.PHONY: qemu-debug
qemu-debug:
	@$(QEMU) -gdb tcp::8864 -drive format=raw,file=$(OUTPUT_RAW_DISK) -S -d guest_errors -d int -no-reboot -no-shutdown
	@# Help: Runs QEMU in debug mode so that we can debug the bootloader

.PHONY: gdb-debug
gdb-debug:
	@$(GDB) -x $(DIR_SCRIPTS)/gdb/debug_commands.txt
	@# Help: Runs GDB with some personal preferences to debug the bootloader

.PHONY: test
test:
	@$(QEMU) -drive format=raw,file=$(OUTPUT_RAW_DISK) -d guest_errors -d int -no-reboot -no-shutdown
	@# Help: Runs QEMU without debugging settings

.PHONY: clean
clean:
	@rm -rf $(DIR_BUILD)
	@echo "[clean] Generated files deleted"
	@# Help: Clean all generated files

.PHONY: build
build:
	@echo "[build] Building AlmeidaOS"
	@$(DIR_DOCKER)/build.sh all
	@# Help: Build the OS using a docker container

MAKEOVERRIDES =
help:
	@printf "%-20s %s\n" "Target" "Description"
	@printf "%-20s %s\n" "------" "-----------"
	@make -pqR : 2>/dev/null \
        	| awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' \
        	| sort \
        	| egrep -v -e '^[^[:alnum:]]' -e '^$@$$' \
        	| xargs -I _ sh -c 'printf "%-20s " _; make _ -nB | (grep -i "^# Help:" || echo "") | tail -1 | sed "s/^# Help: //g"'
