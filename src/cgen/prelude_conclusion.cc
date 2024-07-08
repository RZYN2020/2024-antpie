// #include "prelude_conclusion.hh"

// // https://stackoverflow.com/questions/3407012/rounding-up-to-the-nearest-multiple-of-a-number
// int roundUp(int numToRound, int multiple) {
//   assert(multiple && ((multiple & (multiple - 1)) == 0));
//   return (numToRound + multiple - 1) & -multiple;
// }

// void prelude_conclusion(MModule *mod) {
//   for (auto &func : mod->getFunctions()) {

//     //    patch addi s0 x? ? (translated from alloca)
//     //    patch store, load  (from spill process)
//     for (auto &bb : func->getBasicBlocks()) {
//       for (auto &i : bb->getInstructions()) {
//         // std::cout << "check: " + i->to_string() << std::endl;
//         if (i->getInsTag() == MInstruction::ADDIW ||
//             i->getInsTag() == MInstruction::ADDI) {
//           // std::cout << "  got: " + i->to_string() << std::endl;
//           // std::cout << "  got: REG " + i->getReg(0)->getName() << "ADDR: " +
//           // std::to_string((uint64_t)i->getReg(0))<< std::endl; std::cout << "
//           // got: !!! " + i->getReg(0)->getName() << "ADDR: " +
//           // std::to_string((uint64_t)Register::reg_s0)<< std::endl;
//           // if (i->getReg(0) == Register::reg_s0) {
//           //   std::cout << "  IS S0" << std::endl;
//           // } else {
//           //   std::cout << "  NO S0" << std::endl;
//           // }
//           if (i->getReg(0) == Register::reg_s0) {
//             // std::cout << "    got s0: " + i->to_string() << std::endl;
//             // std::cout << i->to_string() << std::endl;
//             i->setImm(i->getImm() - func->getSavedSize());
//             // std::cout << i->to_string() << std::endl << std::endl;
//           }
//         }
//         if (i->getInsTag() == MInstruction::SD ||
//             i->getInsTag() == MInstruction::SW ||
//             i->getInsTag() == MInstruction::FSW) {
//           if (i->getReg(1) == Register::reg_s0) {
//             i->setImm(i->getImm() - func->getSavedSize());
//           }
//         }
//         if (i->getInsTag() == MInstruction::LD ||
//             i->getInsTag() == MInstruction::LW ||
//             i->getInsTag() == MInstruction::FLW) {
//           if (i->getReg(0) == Register::reg_s0) {
//             i->setImm(i->getImm() - func->getSavedSize());
//           }
//         }
//       }
//     }

//     // 1. add prelude for each function
//     uint32_t stack_zs =
//         roundUp(func->getSavedSize() + func->getSpilledSize(), 16);
//     // Suppose the first bb is the entry
//     auto entry = func->getBasicBlocks().at(0).get();

//     auto sub_sp = new MIaddi(Register::reg_sp, -stack_zs, Register::reg_sp);
//     auto store_ra = new MIsd(Register::reg_ra, stack_zs - 8, Register::reg_sp);
//     auto store_fp = new MIsd(Register::reg_s0, stack_zs - 16, Register::reg_sp);
//     auto get_fp = new MIaddi(Register::reg_sp, stack_zs, Register::reg_s0);
//     // no saved registers currently
//     entry->pushInstrsAtHead({sub_sp, store_ra, store_fp, get_fp});

//     // 2. add exit block for each function
//     auto exit = new MBasicBlock(func->getName() + "_exit");
//     auto load_ra = new MIld(Register::reg_sp, stack_zs - 8, Register::reg_ra);
//     auto load_fp = new MIld(Register::reg_sp, stack_zs - 16, Register::reg_s0);
//     auto add_sp = new MIaddi(Register::reg_sp, stack_zs, Register::reg_sp);
//     auto ret = new MIret();
//     exit->pushInstrs({load_ra, load_fp, add_sp, ret});

//     for (auto &bb : func->getBasicBlocks()) {
//       for (auto &i : bb->getInstructions()) {
//         if (i->getInsTag() == MInstruction::RET) {
//           i->replaceWith({new MIj(exit)});
//         }
//       }
//     }

//     func->pushBasicBlock(exit);
//   }
// }
