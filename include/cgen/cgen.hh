#pragma once
#include "Machine.hh"
#include "Module.hh"

void generate_code(MModule* res, ANTPIE::Module* ir);
void allocate_register(MModule* mod);
void prelude_conclusion(MModule* mod);
void select_instruction(MModule* res, ANTPIE::Module* ir);
