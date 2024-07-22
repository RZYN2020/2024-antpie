#include "allocate_register.hh"
#include <set>

void spill_registers(MFunction *func, map<Register *, int> *spill,
                     int &stack_offset) {
  // allocate arguments
  auto ftp = func->getType();
  for (int i = 0; i < func->getType()->getArgSize(); i++) {
    auto arg = func->getPara(i);
    if (arg->getRegister() == nullptr) {
      spill->insert({arg, -arg->getOffset()});
    } else {
      if (arg->isPointer()) {
        stack_offset += 8;
      } else {
        stack_offset += 4;
      }
      spill->insert({arg, stack_offset});
    }
  }
  // allocate registers
  for (auto &bb : func->getBasicBlocks()) {
    for (auto &ins : bb->getPhis()) {
      auto reg = ins.get();
      if (reg->isPointer()) {
        stack_offset += 8;
      } else {
        stack_offset += 4;
      }
      spill->insert({reg, stack_offset});
    }
    for (auto &ins : bb->getInstructions()) {
      if (ins->getTarget() != ins.get()) { // not used as register
        continue;
      }
      auto reg = ins.get();
      if (reg->isPointer()) {
        stack_offset += 8;
      } else {
        stack_offset += 4;
      }
      spill->insert({reg, stack_offset});
    }
  }
}

static void lower_call_spill_only(MFunction *func, map<Register *, int> *spill,
                                  int &stack_offset) {
  // std::cout << "lower_call_spill_only start" << endl;
  map<Register *, Register *> allocation;
  set<Register *> caller_saved;
  for (auto &bb : func->getBasicBlocks()) {
    vector<MInstruction *> instrs;
    for (auto &ins : bb->getInstructions()) {
      instrs.push_back(&*ins);
    }
    for (auto ins : instrs) {
      if (ins->getInsTag() == MInstruction::H_CALL) {
        // std::cout << "spill " << *ins << endl;
        auto call = static_cast<MHIcall *>(ins);
        func->reg_pool->push_back(ins->replaceWith(call->generateCallSequence(
            func, stack_offset, spill, &allocation, &caller_saved)));
      }
    }
  }
  // std::cout << "lower_call_spill_only over" << endl;
}

void spill_register_for_func(MFunction *func) {
  auto spill = make_unique<map<Register *, int>>();
  map<Register *, Register *> allocation;
  set<Register *> callee_saved;
  int stack_offset = 16; // with ra and sp

  spill_registers(func, spill.get(), stack_offset);

  // for (const auto &pair : *spill) {
  //   std::cout << "Register ID: " << pair.first->getName() << ", Offset: " << pair.second
  //             << std::endl;
  // }

  // std::cout << "lower_alloca" << endl;
  lower_alloca(func, stack_offset);
  // std::cout << "lower_call_spill_only" << endl;

  lower_call_spill_only(func, spill.get(), stack_offset);
  add_prelude(func, &allocation, spill.get(), stack_offset, &callee_saved);
  add_conclude(func, &allocation, spill.get(), stack_offset, &callee_saved);
  rewrite_program_spill(func, spill.get());
  fixRange(func);
}

void spill_all_register(MModule *mod) {
  for (auto &func : mod->getFunctions()) {
    out_of_ssa(func.get());
    spill_register_for_func(func.get());
  }
  mod->ssa_out();
}
