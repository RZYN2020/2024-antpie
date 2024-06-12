#include "ir/Instruction.hh"
#include "ir/Function.hh"
#include "ir/BasicBlock.hh"
#include "ir/Module.hh"
#include "ir/Constant.hh"
#include "asm/Machine.hh"
#include "cgen/select_instruction.hh"
#include "cgen/allocate_register.hh"
#include "cgen/prelude_conclusion.hh"

#include <fstream>
#include <iostream>