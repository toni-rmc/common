# user defined variables

INCLUDES =

# OS specific utility programs

RM = rm
RD = rmdir /s /q
MD = mkdir
CP = copy /Y

# override built-in variables

CC = gcc
COMPILE.c = $(CC) $(CFLAGS) $(INCLUDES) $(CPPFLAGS) $(TARGET_ARCH) -c

# output directory for object and dependency files
OUTPUT_DIR := ../out

# binary directory for executeable
BIN_DIR := ../bin

# build directories

ifneq "$(MAKECMDGOALS)" "clean"
      make_output_dir := $(shell $(MD) "$(OUTPUT_DIR)")
      make_bin_dir := $(shell $(MD) "$(BIN_DIR)")
endif

sources := $(wildcard *.c) # get source files

objects := $(subst .c,.o,$(addprefix $(OUTPUT_DIR)/, $(sources)))

dependencies = $(subst .o,.d, $(objects))

programs := $(BIN_DIR)/common

# file to test
TEST_FILES := ../test/file1.txt ../test/file2.txt

all: $(programs)
	@echo -------------------------------
	@"$(programs)" $(TEST_FILES)

$(programs): $(objects)
	$(LINK.o) $^ $(OUTPUT_OPTION)

# include other makefiles

ifneq "$(MAKECMDGOALS)" "clean"           # prevent dependency file generation when command-line target 'clean' is given
    include $(dependencies)
endif

# generate dependency files

$(OUTPUT_DIR)/%.d: %.c
	$(COMPILE.c) -MM -MF $@ -MP -MT $(subst .d,.o,$@) $<


# build object files in output directory

$(OUTPUT_DIR)/%.o: %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<


# phony targets folow

.PHONY: clean

clean:
	-$(RM) $(OUTPUT_DIR)/*.o $(OUTPUT_DIR)/*.d $(BIN_DIR)/*.exe
	$(RD) "$(OUTPUT_DIR)"
	$(RD) "$(BIN_DIR)"

