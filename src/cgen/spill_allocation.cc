#include "allocate_register.hh"
#include <set>

void allocate_registers(MFunction *func, map<Register *, int> *spill,
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

static void lower_call(MFunction *func, map<Register *, int> *spill,
                       int &stack_offset) {
  map<Register *, Register *> allocation;
  set<Register *> caller_saved;
  for (auto &bb : func->getBasicBlocks()) {
    for (auto &ins : bb->getInstructions()) {
      if (ins->getInsTag() == MInstruction::H_CALL) {
        auto call = static_cast<MHIcall *>(ins.get());
        ins->replaceWith(call->generateCallSequence(
            func, stack_offset, spill, &allocation, &caller_saved));
      }
    }
  }
}

void spill_register_for_func(MFunction *func) {

  // std::cout << "spill_register_for_func " << func->getName() << endl;

  // 3.1 allocate register
  int stack_offset = 16; // with ra and sp
  auto spill = make_unique<map<Register *, int>>();
  allocate_registers(func, spill.get(), stack_offset);

  // for (auto it = spill->begin(); it != spill->end(); ++it) {
  //   std::cout << "Reg: " << it->first->getName() << ", Addr: " << it->second
  //             << std::endl;
  // }

  // 3.2 rewrite program

  lower_alloca(func, stack_offset);
  lower_call(func, spill.get(), stack_offset);
  map<Register *, Register *> allocation;
  set<Register *> callee_saved;
  add_prelude(func, &allocation, spill.get(), stack_offset, &callee_saved);
  add_conclude(func, &allocation, spill.get(), stack_offset, &callee_saved);
  rewrite_program_spill(func, spill.get());
}

void spill_all_register(MModule *mod) {
  // step1. out-of-ssa

  // step2: spill registers
  for (auto &func : mod->getFunctions()) {
    out_of_ssa(func.get());
    spill_register_for_func(func.get());
  }
  mod->ssa_out();
}
