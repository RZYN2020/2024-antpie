#ifndef _SELECT_INSTRUCTION_H_
#define _SELECT_INSTRUCTION_H_

#include <cassert>
#include "../asm/Machine.hh"
#include "../ir/Module.hh"


void select_instruction(MachineModule* res, ANTPIE::Module* ir);

#endif

