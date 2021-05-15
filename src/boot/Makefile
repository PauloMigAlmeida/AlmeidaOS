#----------------------------------------------------------------------------
# AlmeidaOS boot loader makefile
#----------------------------------------------------------------------------

DIR_ROOT	:= $(CURDIR)/../../

include $(DIR_ROOT)/scripts/config.mk

DIR_TARGET	:= $(DIR_BUILD)/boot
# ASFLAGS 	:= -f bin

SRC_ASM_FILES	:= $(wildcard *.asm)
BIN_ASM_FILES	:= $(SRC_ASM_FILES:%.asm=$(DIR_TARGET)/%.bin)

TAG 		:= [boot]

all: mkdir $(BIN_ASM_FILES)
	@echo "$(TAG) Compiled successfully"

.PHONY: mkdir
mkdir:
	@mkdir -p $(DIR_TARGET)

.PHONY: clean
clean:
	@rm -f $(BIN_ASM_FILES)

$(BIN_ASM_FILES): $(DIR_TARGET)/%.bin: %.asm
	@echo "$(TAG) Assembling $<"
	@$(AS) $(ASFLAGS) $< -o $@
