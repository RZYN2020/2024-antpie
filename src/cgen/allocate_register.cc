#include "allocate_register.hh"

void out_of_ssa(MachineModule *mod) {
  for (auto &func : mod->getFunctions()) {
    for (auto &bb : func->getBasicBlocks()) {
      std::vector<MachineInstruction *> remove;
      for (auto &ins : bb->getInstructions()) {
        if (ins->getTag() == MachineInstruction::PHI) {
          auto phi = static_cast<MIphi *>(ins.get());
          for (int i = 0; i < phi->getRegNum(); i++) {
            auto reg = phi->getReg(i);
            auto bk = phi->getIncomingBlock(i);
            if (reg->is_float()) {
              auto move = new MIfmv_s(reg, phi);
              bk->pushInstr(move);
            } else {
              auto move = new MImv(reg, phi);
              bk->pushInstr(move);
            }
          }
          remove.push_back(ins.get());
        }
      }
      for (auto &ins : remove) {
        auto phi = bb->removeInstruction(ins);
        mod->pushIntoRegPool(phi.release());
      }
    }
  }
}

void allocate_register(MachineModule *mod) {
  // step1. out-of-ssa
  out_of_ssa(mod);
  // step2. allocate registers
  
}

// Linear Scan Register Allocation on SSA Form
static void allocate_register_ssa(MachineModule *mod) {
  assert(0);
  // step1. Lifetime Analysis
  //// 1.1 Linearize blocks
  //// 1.2 Build Intervals
  // step2. Linear Scan
  // step3. Resolution
}