.IGNORE: llvm-test
.PHONY: clean all

DEBUG ?= 0
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=$(if $(DEBUG),Debug,Release)
ANTLR_PATH = $(shell find /usr/local/lib -name "antlr-*-complete.jar")
ANTLR = java -jar $(ANTLR_PATH) -listener -visitor -long-messages
PFILE = $(shell find . -name "SysYParser.g4")
LFILE = $(shell find . -name "SysYLexer.g4")

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

PORT := 1237
qemu-debug:
	qemu-riscv64 -cpu sifive-u54 -L /usr/riscv64-linux-gnu -g $(PORT) $(EXE) &
	gdb-multiarch -q \
		-ex "set sysroot /usr/riscv64-linux-gnu" \
		-ex "file $(EXE)" \
		-ex "target remote localhost:$(PORT)" \
		-ex "break main" \
		-ex "continue"

clean:
	rm -rf build
	rm -rf ./parser/antlr4/*.cpp
	rm -rf ./parser/antlr4/*.h
	rm -rf ./parser/antlr4/*.tokens
	rm -rf ./parser/antlr4/*.interp

antlr: $(LFILE) $(PFILE)
	$(ANTLR) -Dlanguage=Cpp $(PFILE) $(LFILE)


SRC := tests/test.s
RES := tests/test
gcc-riscv64:
	riscv64-linux-gnu-gcc-10 -o $(RES) $(SRC)

# todo: official docker test
# https://pan.educg.net/#/s/V2oiq?path=%2F