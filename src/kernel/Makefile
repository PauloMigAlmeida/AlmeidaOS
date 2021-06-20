#----------------------------------------------------------------------------
# AlmeidaOS kernel makefile
#----------------------------------------------------------------------------

DIR_ROOT	:= $(CURDIR)/../../

include $(DIR_ROOT)/scripts/config.mk

# override AS flags from $(DIR_ROOT)/scripts/config.mk
ASFLAGS		:= -f elf64

DIR_SRC_SUBSYSTEMS := $(shell find $(CURDIR)/* -maxdepth 1 -type d)
DIR_TARGET	:= $(DIR_BUILD)/kernel

SRC_C_FILES	:= $(wildcard *.c)
BIN_C_FILES	:= $(SRC_C_FILES:%.c=$(DIR_TARGET)/%.o)

SRC_ASM_FILES	:= $(wildcard *.asm)
BIN_ASM_FILES	:= $(SRC_ASM_FILES:%.asm=$(DIR_TARGET)/%.o)

TAG 		:= [kernel]

all: mkdir compile linker
	@echo "$(TAG) Compiled successfully"

.PHONY: mkdir
mkdir:
	@mkdir -p $(DIR_TARGET)

.PHONY: clean
clean:
	@rm -f $(BIN_C_FILES)

.PHONY: compile
compile: $(BIN_C_FILES) $(BIN_ASM_FILES) $(DIR_SRC_SUBSYSTEMS)

$(BIN_C_FILES): $(DIR_TARGET)/%.o: %.c
	@echo "$(TAG) Compiling $<"
	@$(CC) $(CCFLAGS) -I$(DIR_INCLUDE) -c $< -o $@

$(BIN_ASM_FILES): $(DIR_TARGET)/%.o: %.asm
	@echo "$(TAG) Assembling $<"
	@$(AS) $(ASFLAGS) $< -o $@

.PHONY: $(DIR_SRC_SUBSYSTEMS)
$(DIR_SRC_SUBSYSTEMS):
	@$(MAKE) $(MAKE_FLAGS) --directory=$@

.PHONY: linker
linker:
	@echo "$(TAG) Linking Kernel"
	@$(LD) $(LDFLAGS) -T linker.ld -o $(DIR_TARGET)/kernel $(shell find $(DIR_TARGET)/ -type f -name "*.o")
	# split kernel for debug symbol files.
	@$(OBJCOPY) --only-keep-debug $(DIR_TARGET)/kernel $(DIR_TARGET)/kernel.debug
	@$(OBJCOPY) --strip-debug $(DIR_TARGET)/kernel 

