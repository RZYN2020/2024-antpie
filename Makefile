.IGNORE: llvm-test
.PHONY: clean all

DEBUG ?= 0
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(if $(DEBUG),Debug,Release)

all:
	@cmake $(CMAKE_FLAGS) -B build
	@cmake --build build

BIN_DIR = build
IRFile = tests/test.ll
run:
	$(BIN_DIR)/compiler

# Test llvm
# Input: llvm ir
# Output: return value
llvm-test: run
	lli $(IRFile); echo $$?

EXE := tests/test
INPUT := tests/test.in
qemu-run:
	qemu-riscv64 -L /usr/riscv64-linux-gnu -s 1024M $(EXE) < $(INPUT)

clean:
	rm -rf build

SRC := tests/test.c
RES := tests/test
gcc-riscv64:
	riscv64-linux-gnu-gcc-10 -o $(RES) $(SRC)

# todo: official docker test
# https://pan.educg.net/#/s/V2oiq?path=%2F