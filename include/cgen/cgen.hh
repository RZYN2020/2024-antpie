#pragma once
#include "Machine.hh"
#include "Module.hh"

using LivenessInfo = map<MInstruction *, set<Register *>>;

void generate_code(MModule* res, ANTPIE::Module* ir, bool opt);
void select_instruction(MModule* res, ANTPIE::Module* ir);
void ssa_liveness_analysis(MFunction *func, LivenessInfo *liveness_i,
                           LivenessInfo *liveness_f);
void schedule_instruction(MModule *mod);
void allocate_register(MModule *mod);
void spill_all_register(MModule* mod);
void peephole_optimize(MModule* mod);
void branch_simplify(MModule *mod);