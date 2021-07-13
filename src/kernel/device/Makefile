#----------------------------------------------------------------------------
# AlmeidaOS kernel/device makefile
#----------------------------------------------------------------------------

DIR_ROOT	:= $(CURDIR)/../../../

include $(DIR_ROOT)/scripts/config.mk

DIR_SRC_SUBSYSTEMS := $(shell find $(CURDIR)/* -maxdepth 1 -type d)
DIR_TARGET	:= $(DIR_BUILD)/kernel/device

SRC_C_FILES	:= $(wildcard *.c)
BIN_C_FILES	:= $(SRC_C_FILES:%.c=$(DIR_TARGET)/%.o)

TAG 		:= [kernel/device]

all: mkdir compile
	@echo "$(TAG) Compiled successfully"

.PHONY: mkdir
mkdir:
	@mkdir -p $(DIR_TARGET)

.PHONY: clean
clean:
	@rm -f $(BIN_C_FILES)

.PHONY: compile
compile: $(BIN_C_FILES) $(DIR_SRC_SUBSYSTEMS)

$(BIN_C_FILES): $(DIR_TARGET)/%.o: %.c
	@echo "$(TAG) Compiling $<"
	@$(CC) $(CCFLAGS) -I$(DIR_INCLUDE) -c $< -o $@

.PHONY: $(DIR_SRC_SUBSYSTEMS)
$(DIR_SRC_SUBSYSTEMS):
	@$(MAKE) $(MAKE_FLAGS) --directory=$@
