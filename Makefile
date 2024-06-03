# Makefile

# Compiler settings
CC = clang++
DEBUG ?= 0
ifeq ($(DEBUG), 1)
  CFLAGS = --std=c++17 -g  -I$(INC_DIR)
else
  CFLAGS = --std=c++17 -O2 -I$(INC_DIR)
endif

# Directories
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/obj

# Source and header files
SRCS = $(wildcard $(SRC_DIR)/*.cc  $(SRC_DIR)/**/*.cc)
OBJS = $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(patsubst $(SRC_DIR)/%.cc,$(INC_DIR)/%.hh,$(SRCS))

# Target
TARGET = $(BIN_DIR)/compiler

# IR file
IRFile = tests/test.ll

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^


ifeq ($(OS),Windows_NT)
# Use Windows-compatible mkdir command
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc $(DEPS)
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@
else
# Linux command
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
endif

run:
	$(BIN_DIR)/compiler

# Test llvm
# Input: llvm ir
# Output: return value
llvm-test: run
	tests/test_script.sh $(IRFile)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)