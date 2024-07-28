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

        //////////////////////////////////////
        // li a0, xx # ASSIGN TO _t0
        // li a1, xx # ASSIGN TO _t1
        // mul a2, a0, a1 # ASSIGN TO _t2
        // ---------------------------
        // ==> mv a2 a1
        //////////////////////////////////////
        if (ins->getInsTag() == MInstruction::LI) {
          if (i < (inss.size() - 2) &&
              inss[i + 1]->getInsTag() == MInstruction::LI) {
            if (inss[i + 2]->getInsTag() == MInstruction::MUL &&
                inss[i + 2]->getReg(0) == ins->getTarget() &&
                inss[i + 2]->getReg(1) == inss[i + 1]->getTarget()) {
              auto l0 = static_cast<MIli *>(ins);
              auto l1 = static_cast<MIli *>(inss[i + 1]);
              auto newi = new MIli(l0->imm * l1->imm, inss[i + 2]->getTarget());
              ins->replaceWith({newi});
              inss[i + 1]->replaceWith({});
              inss[i + 2]->replaceWith({});
            // std::cout << "match " << " mul a0, a0" << endl;
              i += 3;
              continue;
            }
          }
        }

        //////////////////////////////////////
        // li a0, xx # ASSIGN TO _t0
        // subw a2, a1, a0 # ASSIGN TO _t2
        // ---------------------------
        // ==> addiw a2 -xx
        //////////////////////////////////////
        int lo = -2048;
        int hi = 2047;
        if (ins->getInsTag() == MInstruction::LI) {
          if (i < (inss.size() - 1) &&
              inss[i + 1]->getInsTag() == MInstruction::SUBW &&
              inss[i + 1]->getReg(1) == ins->getTarget()) {
            auto li = -static_cast<MIli *>(ins)->imm;
            if (li < hi && li > lo) {
              auto newi = new MIaddi(inss[i + 1]->getReg(0), li,
                                     inss[i + 1]->getTarget());
              ins->replaceWith({newi});
              i += 2;
            // std::cout << "match " << " subw a0, a0" << endl;
              continue;
            }
          }
        }
        i += 1;
      }
    }
  }
}