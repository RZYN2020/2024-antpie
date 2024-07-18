#include "Machine.hh"

transform_gep = {
    match(li ?, 1, ds)
    match(li ?, 1, ds)
    match(mul )
}


instr 0 = li x1 0
instr 1 = li x2 ?
instr 2 = mul x3 x1 x2





void peephole_optimize(MModule *mod) {
  for (auto &func : mod->getFunctions()) {
    for (auto &bb : func->getBasicBlocks()) {
        for (auto &ins : bb->getAllInstructions()) {

        }
    }
  }
}