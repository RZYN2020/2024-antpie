#pragma once
#include "Machine.hh"
#include "Module.hh"

void generate_code(MModule* res, ANTPIE::Module* ir);
void select_instruction(MModule* res, ANTPIE::Module* ir);
void allocate_register(MModule *mod);
void spill_all_register(MModule* mod);
void peephole_optimize(MModule* mod);