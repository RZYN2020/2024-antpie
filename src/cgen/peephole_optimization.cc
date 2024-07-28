#include "Machine.hh"

void peephole_optimize(MModule *mod) {
  for (auto func : mod->getFunctions()) {
    for (auto bb : func->getBasicBlocks()) {
      auto inss = bb->getAllInstructions();
      int i = 0;
      while (i < inss.size()) {
        auto ins = inss[i];

        //////////////////////////////////////
        // mv a0, a0
        // ---------------------------
        // ==> nop
        //////////////////////////////////////
        if (ins->getInsTag() == MInstruction::MV) {
          if (ins->getTarget() == ins->getReg(0)) {
            ins->replaceWith({});
            i++;
            // std::cout << "match " << " mv a0, a0" << endl;
            continue;
          }
        }

        // todo: delete redudent pop and push between calls...
        i += 1;
      }
    }
  }
}