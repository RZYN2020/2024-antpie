#include "prelude_conclusion.hh"

// https://stackoverflow.com/questions/3407012/rounding-up-to-the-nearest-multiple-of-a-number
int roundUp(int numToRound, int multiple) {
  assert(multiple && ((multiple & (multiple - 1)) == 0));
  return (numToRound + multiple - 1) & -multiple;
}

void prelude_conclusion(MachineModule *mod) {
  for (auto &func : mod->getFunctions()) {
    // 1. add prelude for each function
    uint32_t stack_zs =
        roundUp(func->getSavedSize() + func->getSpilledSize(), 16);
    // Suppose the first bb is the entry
    auto entry = func->getBasicBlocks().at(0).get();
    auto sub_sp = new MIaddiw(&reg_sp, stack_zs, &reg_sp);
    auto store_ra = new MIsd(&reg_sp, 8, &reg_ra);
    auto store_fp = new MIsd(&reg_sp, 16, &reg_s0);
    // no saved registers currently
    entry->pushInstrsAtHead({sub_sp, store_ra, store_fp});

    // 2. add exit block for each function
    auto exit = new MachineBasicBlock(func->getName() + "_exit");
    auto load_ra = new MIld(&reg_sp, 8, &reg_ra);
    auto load_fp = new MIld(&reg_sp, 16, &reg_s0);
    auto add_sp = new MIaddiw(&reg_sp, stack_zs, &reg_sp);
    auto ret = new MIret();
    exit->pushInstrs({load_ra, load_fp, add_sp, ret});

    for (auto &bb : func->getBasicBlocks()) {
      for (auto &i : bb->getInstructions()) {
        if (i->getTag() == MachineInstruction::RET) {
          i->replaceWith({new MIj(exit)});
        }
      }
    }

    func->pushBasicBlock(exit);
    // 3. patch lw for global variable
  }
}
