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


bool allocate_register_(Register *reg) {
  // // Try to allocate a physical register for the given register
  // PhysicalRegister *phys_reg = find_available_register();
  // if (phys_reg != nullptr) {
  //   reg->setPhysicalRegister(phys_reg);
  //   return true;
  // }
  // return false;
}

void spill_register(Register *reg) {
  // Spill the given register by storing its value to memory
  // and updating all its uses to refer to the spilled location
  // ...
}


void allocate_register(MachineModule *mod) {
  // step1. out-of-ssa
  out_of_ssa(mod);

  // Step 2: Allocate registers
  for (auto &func : mod->getFunctions()) {
    for (auto &bb : func->getBasicBlocks()) {
      for (auto &instr : bb->getInstructions()) {
        if (instr->getTarget() != nullptr) {
          if (!allocate_register_(instr->getTarget())) {
            spill_register(instr->getTarget());
          }
        }
      }
    }
  }
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