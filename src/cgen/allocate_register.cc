#include "Machine.hh"

void out_of_ssa(MModule *mod) {
  for (auto &func : mod->getFunctions()) {
    for (auto &bb : func->getBasicBlocks()) {
      for (auto &phi : bb->getPhis()) {
        for (int i = 0; i < phi->getRegNum(); i++) {
          auto reg = phi->getReg(i);
          auto pre = phi->getIncomingBlock(i);
          auto move = new MImv(reg, phi.get());
          // If critical path
          if (pre->getOutgoings().size() > 1 && bb->getIncomings().size() > 1) {
            auto newbb = func->addBasicBlock("cirtical" + pre->getName()  + bb->getName());
            newbb->pushInstr(move);
            newbb->pushJmp(new MIj(&*bb));

            pre->replaceOutgoing(&*bb, newbb);
            bb->replacePhiIncoming(pre, newbb);
          } else {
            pre->pushInstr(move);
          }
        }
      }
    }
  }
  mod->ssa_out();
}

// bool allocate_register_(Register *reg) {
//   if (reg->getTag() != Register::V_REGISTER) {
//     return true;
//   }
//   return false;
// }

// static bool check_avalibale(Register *reg, MInstruction *ins) {
//   if (ins->getTarget() == reg) {
//     return false;
//   }
//   for (int i = 0; i < ins->getRegNum(); i++) {
//     if (ins->getReg(i) == reg) {
//       return false;
//     }
//   }
//   return true;
// }

// static Register *find_avaliable_register(MInstruction *ins,
//                                          Register *reg) {
//   int reg_cnt = 0;
//   vector<Register *> attempts;
//   if (reg->is_float()) {
//     attempts = {Register::reg_ft0, Register::reg_ft1, Register::reg_ft2};

//   } else {
//     attempts = {Register::reg_t0, Register::reg_t1, Register::reg_t2};
//   }
//   for (auto attempt : attempts) {
//     if (check_avalibale(attempt, ins)) {
//       return attempt;
//     }
//   }
//   assert(0);
// }

// void spill_register(MFunction *func, Register *reg) {

//   auto ins = static_cast<MInstruction *>(reg);
//   if (ins->is_64bit()) {
//     func->incSpilledSize(8);
//   } else {
//     func->incSpilledSize(4);
//   }
//   // std::cout << "   Who use " + ins->getName() + ":" << std::endl;
//   for (auto use : ins->getUses()) {
//     Register *new_reg = find_avaliable_register(use, ins);
//     // std::cout << "    " + use_instr->to_string() << std::endl;
//     if (ins == use->getTarget()) {
//       // std::cout << "      try store" << std::endl;
//       MInstruction *store;
//       if (ins->is_float()) {
//         store = new MIfsw(new_reg, -func->getSpilledSize(),
//         Register::reg_s0);
//       } else if (ins->is_64bit()) {
//         store = new MIsd(new_reg, -func->getSpilledSize(), Register::reg_s0);
//       } else {
//         store = new MIsw(new_reg, -func->getSpilledSize(), Register::reg_s0);
//       }
//       use->replaceRegister(ins, new_reg);
//       use->insertAfter({store});
//     } else {
//       // std::cout << "      try load" << std::endl;
//       MInstruction *load;
//       if (ins->is_float()) {
//         load = new MIflw(Register::reg_s0, -func->getSpilledSize(), new_reg);
//       } else if (ins->is_64bit()) {
//         load = new MIld(Register::reg_s0, -func->getSpilledSize(), new_reg);
//       } else {
//         load = new MIlw(Register::reg_s0, -func->getSpilledSize(), new_reg);
//       }
//       use->replaceRegister(ins, new_reg);
//       use->insertBefore({load});
//     }
//   }
// }

void allocate_register(MModule *mod) {
  // step1. out-of-ssa
  out_of_ssa(mod);

  // step3: spill registers
  // 3.1 allocate register
  // 3.2 rewrite program
  // for (auto &func : mod->getFunctions()) {
  //   for (auto &bb : func->getBasicBlocks()) {
  //     vector<MInstruction *> instrs;
  //     for (auto &instr : bb->getInstructions()) {
  //       instrs.push_back(instr.get());
  //     }
  //     for (auto instr : instrs) {
  //       // std::cout << "before check" << std::endl;
  //       // std::cout << "jjjjjjjjjjjjjjjj\n";
  //       // std::cout << "check " + instr->to_string() << std::endl;
  //       if (instr->getTarget() == instr) {
  //         // std::cout << " get!" << std::endl;
  //         if (!allocate_register_(instr)) {
  //           // std::cout << " get!!!!" << std::endl;
  //           spill_register(func.get(), instr->getTarget());
  //           // std::cout << "   NEXT    " << std::endl;
  //         }
  //       }
  //     }
  //   }
  // }

  // step3. lower ret, call, and alloca

  // step4. function prelude and conclude
}

// Linear Scan Register Allocation on SSA Form
static void allocate_register_ssa(MModule *mod) {
  assert(0);
  // step1. Lifetime Analysis
  //// 1.1 Linearize blocks
  //// 1.2 Build Intervals
  // step2. Linear Scan
  // step3. Resolution
}