#include "Machine.hh"

//////////////////////////////////////
// li a0, 1 # ASSIGN TO _t0
// li a1, 4 # ASSIGN TO _t1
// mul a2, a0, a1 # ASSIGN TO _t2
// ---------------------------
// ==> mv a2 a1
//////////////////////////////////////

//////////////////////////////////////
// li a0, 1 # ASSIGN TO _t0
// li a1, 4 # ASSIGN TO _t1
// mul a2, a0, a1 # ASSIGN TO _t2
// ---------------------------
// ==> mv a2 a1
//////////////////////////////////////

//////////////////////////////////////
// mv a0, a0
// ---------------------------
// ==> nop
//////////////////////////////////////

void peephole_optimize(MModule *mod) {
  for (auto &func : mod->getFunctions()) {
    for (auto &bb : func->getBasicBlocks()) {
      for (auto &ins : bb->getAllInstructions()) {
      }
    }
  }