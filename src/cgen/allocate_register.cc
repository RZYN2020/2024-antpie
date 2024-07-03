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
        phi->removeUse(phi.get()); // phi become a sole VRegister
        for (int i = 0; i < phi->getRegNum(); i++) {
          phi->getReg(i)->removeUse(phi.get());
        }
        func->pushIntoRegPool(phi.release());
      }
    }
  }
}

bool allocate_register_(Register *reg) {
  if (reg->getTag() != Register::V_REGISTER) {
    return true;
  }
  return false;
}

static bool check_avalibale(Register *reg, MachineInstruction *ins) {
  if (ins->getTarget() == reg) {
    return false;
  }
  for (int i = 0; i < ins->getRegNum(); i++) {
    if (ins->getReg(i) == reg) {
      return false;
    }
  }
  return true;
}

static Register *find_avaliable_register(MachineInstruction *ins) {
  int reg_cnt = 0;
  vector<Register *> attempts;
  if (ins->is_float()) {
    attempts = {&reg_ft0, &reg_ft1, &reg_ft2};

  } else {
    attempts = {&reg_t0, &reg_t1, &reg_t2};
  }
  for (auto attempt : attempts) {
    if (check_avalibale(attempt, ins)) {
      return attempt;
    }
  }
}

void spill_register(MachineFunction *func, Register *reg) {
  func->incSpilledSize(4);
  auto ins = static_cast<MachineInstruction *>(reg);

  // std::cout << "   Who use " + ins->getName() + ":" << std::endl;
  for (auto use : ins->getUses()) {
    Register *new_reg = find_avaliable_register(use);
    // std::cout << "    " + use_instr->to_string() << std::endl;
    if (ins == use->getTarget()) {
      // std::cout << "      try store" << std::endl;
      MachineInstruction *store;
      if (ins->is_float()) {
        store = new MIfsw(&reg_s0, new_reg);
      } else {
        store = new MIsw(&reg_s0, new_reg);
      }
      use->replaceRegister(ins, new_reg);
      store->setImm(func->getSpilledSize());
      use->insertAfter({store});
    } else {
      // std::cout << "      try load" << std::endl;
      MachineInstruction *load;
      if (ins->is_float()) {
        load = new MIflw(&reg_s0, new_reg);
      } else {
        load = new MIlw(&reg_s0, new_reg);
      }
      use->replaceRegister(ins, new_reg);
      load->setImm(func->getSpilledSize());
      use->insertBefore({load});
    }
  }
}

void allocate_register(MachineModule *mod) {
  // step1. out-of-ssa
  out_of_ssa(mod);

  // // Step 2: Allocate registers
  for (auto &func : mod->getFunctions()) {
    for (auto &bb : func->getBasicBlocks()) {
      vector<MachineInstruction *> instrs;
      for (auto &instr : bb->getInstructions()) {
        instrs.push_back(instr.get());
      }
      for (auto instr : instrs) {
        // std::cout << "before check" << std::endl;
        // std::cout << "jjjjjjjjjjjjjjjj\n";
        // std::cout << "check " + instr->to_string() << std::endl;
        if (instr->getTarget() == instr) {
          // std::cout << " get!" << std::endl;
          if (!allocate_register_(instr)) {
            // std::cout << " get!!!!" << std::endl;
            spill_register(func.get(), instr->getTarget());
            // std::cout << "   NEXT    " << std::endl;
          }
        }
      }
    }
  }

  // std::cout << " NEXTNEXTNEXT" << std::endl;

  for (auto &func : mod->getFunctions()) {
    for (auto &reg : func->getRegPool()) {
      // std::cout << "check " + reg->to_string() << std::endl;
      if (!allocate_register_(reg.get())) {
        spill_register(func.get(), reg.get());
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