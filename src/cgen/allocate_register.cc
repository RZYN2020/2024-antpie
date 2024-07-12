#include "DomTree.hh"
#include "Machine.hh"
#include <set>

using std::pair;
using std::set;

using LivenessInfo = map<MInstruction *, set<Register *>>;

#define GETUSES(NAME, RegisterTag)                                             \
  std::vector<Register *> getUses##NAME(MInstruction *ins) {                   \
    std::vector<Register *> uses;                                              \
    switch (ins->getInsTag()) {                                                \
    case MInstruction::H_PHI: {                                                \
      assert(0);                                                               \
      break;                                                                   \
    }                                                                          \
    case MInstruction::H_CALL: {                                               \
      auto call = static_cast<MHIcall *>(ins);                                 \
      for (int i = 0; i < call->getArgNum(); i++) {                            \
        auto arg = call->getArg(i);                                            \
        if (arg.tp == MIOprandTp::Reg &&                                       \
            arg.arg.reg->getTag() == Register::RegisterTag) {                  \
          uses.push_back(arg.arg.reg);                                         \
        }                                                                      \
      }                                                                        \
      break;                                                                   \
    }                                                                          \
    case MInstruction::H_RET: {                                                \
      auto ret = static_cast<MHIret *>(ins);                                   \
      auto retv = ret->r;                                                      \
      if (retv.tp == MIOprandTp::Reg &&                                        \
          retv.arg.reg->getTag() == Register::RegisterTag) {                   \
        uses.push_back(retv.arg.reg);                                          \
      }                                                                        \
      break;                                                                   \
    }                                                                          \
    default: {                                                                 \
      for (int i = 0; i < ins->getRegNum(); i++) {                             \
        auto reg = ins->getReg(i);                                             \
        if (reg->getTag() == Register::RegisterTag) {                          \
          uses.push_back(reg);                                                 \
        }                                                                      \
      }                                                                        \
      break;                                                                   \
    }                                                                          \
    }                                                                          \
    return uses;                                                               \
  }

GETUSES(V_FRegister, V_FREGISTER)
GETUSES(V_IRegister, V_IREGISTER)

#define GETDEFS(NAME, RegisterTag)                                             \
  std::vector<Register *> getDefs##NAME(MInstruction *ins) {                   \
    std::vector<Register *> defs;                                              \
    if (ins->getTarget() != nullptr &&                                         \
        ins->getTarget()->getTag() == Register::RegisterTag) {                 \
      defs.push_back(ins->getTarget());                                        \
    }                                                                          \
    return defs;                                                               \
  }

GETDEFS(V_FRegister, V_FREGISTER)
GETDEFS(V_IRegister, V_IREGISTER)

#define GETPHIDEFS(NAME, RegisterTag)                                          \
  std::vector<Register *> getPhiDefs##NAME(MBasicBlock *bb) {                  \
    std::vector<Register *> defs;                                              \
    for (auto &phi : bb->getPhis()) {                                          \
      if (phi->getTarget()->getTag() == Register::RegisterTag) {               \
        defs.push_back(phi->getTarget());                                      \
      }                                                                        \
    }                                                                          \
    return defs;                                                               \
  }

GETPHIDEFS(V_FRegister, V_FREGISTER)
GETPHIDEFS(V_IRegister, V_IREGISTER)

#define SCAN_BACK(NAME, GET_DEFS, GET_PHIS)                                    \
  void scan_back##NAME(Register *r, MBasicBlock *bb,                           \
                       vector<MInstruction *> instrs, int pos,                 \
                       map<MInstruction *, set<Register *>> *liveness) {       \
    for (int i = pos; i >= 0; i--) {                                           \
      auto ins = instrs[i];                                                    \
      if ((*liveness)[ins].find(r) != (*liveness)[ins].end()) {                \
        return;                                                                \
      }                                                                        \
      auto defRegs = GET_DEFS(ins);                                            \
      for (auto defReg : defRegs) {                                            \
        if (defReg == r) {                                                     \
          return;                                                              \
        }                                                                      \
      }                                                                        \
      (*liveness)[ins].insert(r);                                              \
    }                                                                          \
    vector<Register *> phiDefs = GET_PHIS(bb);                                 \
    for (auto phiDef : phiDefs) {                                              \
      if (phiDef == r) {                                                       \
        return;                                                                \
      }                                                                        \
    }                                                                          \
    for (auto pred : bb->getIncomings()) {                                     \
      scan_back##NAME(r, pred, pred->getAllInstructions(),                     \
                      pred->getAllInstructions().size() - 1, liveness);        \
    }                                                                          \
  }

SCAN_BACK(F, getDefsV_FRegister, getPhiDefsV_FRegister)
SCAN_BACK(I, getDefsV_IRegister, getPhiDefsV_IRegister)

// todo: two pass liveness analysis:
// https://inria.hal.science/inria-00558509v2/document
// https://dl.acm.org/doi/abs/10.1007/978-3-319-07959-2_15
// We use a simple Path Exploration Algorithm currently
void ssa_liveness_analysis(MFunction *func, LivenessInfo *liveness_i,
                           LivenessInfo *liveness_f) {
  for (auto &bb : func->getBasicBlocks()) {
    auto instrs = bb->getAllInstructions();
    for (auto ins : instrs) {
      (*liveness_i)[ins] = set<Register *>();
      (*liveness_f)[ins] = set<Register *>();
    }
  }

  for (auto &bb : func->getBasicBlocks()) {
    auto instrs = bb->getAllInstructions(); // without phi
    for (int i = static_cast<int>(instrs.size()) - 1; i >= 0; i--) {
      auto ins = instrs[i];
      for (auto r : getUsesV_IRegister(ins)) {
        scan_backI(r, bb.get(), instrs, i, liveness_i);
      }
      for (auto r : getUsesV_FRegister(ins)) {
        scan_backF(r, bb.get(), instrs, i, liveness_f);
      }
    }
    // a lazy handle of phi function...
    // merge the program point of all phi and the first instuction together
    auto first = instrs[0];
    for (auto &phi : bb->getPhis()) {
      for (int i = 0; i < phi->getOprandNum(); i++) {
        auto pred = phi->getIncomingBlock(i);
        auto opd = phi->getOprand(i);
        if (opd.tp == MIOprandTp::Reg) {
          if (opd.arg.reg->getTag() == Register::V_IREGISTER) {
            (*liveness_i)[first].insert(opd.arg.reg);
            scan_backI(static_cast<FRegister *>(opd.arg.reg), pred,
                       pred->getAllInstructions(),
                       pred->getAllInstructions().size() - 1, liveness_f);
          } else {
            (*liveness_f)[first].insert(opd.arg.reg);
            scan_backF(static_cast<IRegister *>(opd.arg.reg), pred,
                       pred->getAllInstructions(),
                       pred->getAllInstructions().size() - 1, liveness_i);
          }
        }
      }
    }
  }
}

void printLivenessInfo(MFunction *func, LivenessInfo *liveness_ireg,
                       LivenessInfo *liveness_freg) {
  std::cout << "Liveness Information for Function: " << func->getName()
            << std::endl;

  std::cout << "Integer Register Liveness:" << std::endl;
  for (auto &pair : *liveness_ireg) {
    std::cout << "Instruction: " << *pair.first << std::endl;
    std::cout << "   LiveIn: ";
    for (Register *reg : pair.second) {
      std::cout << reg->getName() << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl << std::endl;

  std::cout << "Floating-Point Register Liveness:" << std::endl;
  for (auto &pair : *liveness_freg) {
    std::cout << "Instruction: " << *pair.first << std::endl;
    std::cout << "   LiveIn: ";
    for (Register *reg : pair.second) {
      std::cout << reg->getName() << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl << std::endl;
}

#define MAX_I_REG_NUM 23 // without t0 t1 t2, zero ra sp gp tp s0
#define MAX_F_REG_NUM 29 // without ft0 ft1 ft2

void spill_registers(int &stk_offset, map<Register *, int> *spilled,
                     LivenessInfo *liveness_ireg, LivenessInfo *liveness_freg) {
  // random spill now...
  // todo: more spill strategy
  for (auto &pair : *liveness_ireg) {
    vector<Register *> regs;
    for (Register *reg : pair.second) {
      if (spilled->find(reg) != spilled->end()) {
        regs.push_back(reg); // register which haven't been spilled
      }
    }
    while (regs.size() >= MAX_I_REG_NUM) {
      auto reg = static_cast<VRegister *>(*regs.end());
      regs.pop_back();

      if (!reg->isInstruction()) {
        auto para = static_cast<ParaRegister *>(reg);
        spilled->insert({reg, -para->getOffset()});
      } else {
        if (reg->isPointer()) {
          stk_offset += 8;
        } else {
          stk_offset += 4;
        }
        spilled->insert({reg, stk_offset});
      }
    }
  }
  for (auto &pair : *liveness_freg) {
    vector<Register *> regs;
    for (Register *reg : pair.second) {
      if (spilled->find(reg) != spilled->end()) {
        regs.push_back(reg); // register which haven't been spilled
      }
    }
    while (regs.size() >= MAX_F_REG_NUM) {
      auto reg = static_cast<VRegister *>(*regs.end());
      regs.pop_back();
      if (!reg->isInstruction()) {
        auto para = static_cast<ParaRegister *>(reg);
        spilled->insert({reg, -para->getOffset()});
      } else {
        stk_offset += 4;
        spilled->insert({reg, stk_offset});
      }
    }
  }
}

Register *getOneIRegiter(set<Register *> *used) {
  static set<Register *> all_registers = {
      Register::reg_s0, Register::reg_s1, Register::reg_s2,  Register::reg_s3,
      Register::reg_s4, Register::reg_s5, Register::reg_s6,  Register::reg_s7,
      Register::reg_s8, Register::reg_s9, Register::reg_s10, Register::reg_s11,
      Register::reg_t3, Register::reg_t4, Register::reg_t5,  Register::reg_t6,
      Register::reg_a0, Register::reg_a1, Register::reg_a2,  Register::reg_a3,
      Register::reg_a4, Register::reg_a5, Register::reg_a6,  Register::reg_a7,
  };
  for (Register *reg : all_registers) {
    if (used->find(reg) == used->end()) {
      return reg;
    }
  }
  assert(0);
}

Register *getOneFRegiter(set<Register *> *used) {
  static set<Register *> all_registers = {
      Register::reg_ft3,  Register::reg_ft4,  Register::reg_ft5,
      Register::reg_ft6,  Register::reg_ft7,  Register::reg_fs0,
      Register::reg_fs1,  Register::reg_fs2,  Register::reg_fs3,
      Register::reg_fs4,  Register::reg_fs5,  Register::reg_fs6,
      Register::reg_fs7,  Register::reg_fs8,  Register::reg_fs9,
      Register::reg_fs10, Register::reg_fs11, Register::reg_ft8,
      Register::reg_ft9,  Register::reg_ft10, Register::reg_ft11,
      Register::reg_fa0,  Register::reg_fa1,  Register::reg_fa2,
      Register::reg_fa3,  Register::reg_fa4,  Register::reg_fa5,
      Register::reg_fa6,  Register::reg_fa7,
  };
  for (Register *reg : all_registers) {
    if (used->find(reg) == used->end()) {
      return reg;
    }
  }
  assert(0);
}

void setUsedtoLiveIn(MInstruction *ins, set<Register *> *used,
                     map<Register *, Register *> *allocation,
                     LivenessInfo *liveness_ireg, LivenessInfo *liveness_freg) {
  used->clear();
  auto iregs = liveness_ireg->at(ins);
  auto fregs = liveness_freg->at(ins);
  for (auto reg : iregs) {
    used->insert(allocation->at(reg));
  }
  for (auto reg : fregs) {
    used->insert(allocation->at(reg));
  }
}

// void allocate(MFunction *func, map<Register *, Register *> *allocation,
//               map<Register *, int> *spill, LivenessInfo *liveness_ireg,
//               LivenessInfo *liveness_freg) {
//   set<Register *> used;
//   int ireg_cnt = 9;
//   int freg_cnt = 3;
//   // allocate parameters
//   // don't allocate for unused parameters
//   MInstruction *first = func->getEntry()->getInstructions().at(0).get();
//   for (auto reg : liveness_ireg->at(first)) {
//     auto para = static_cast<ParaRegister *>(reg);
//     if (spill->find(para) == spill->end()) {
//       if (para->getRegister() != nullptr &&
//           used.find(para->getRegister()) == used.end()) {
//         allocation->insert({para, para->getRegister()});
//         used.insert(para->getRegister());
//       } else {
//         while (auto tryr = Register::getIRegister(ireg_cnt++)) {
//           if (used.find(tryr) != used.end()) {
//             allocation->insert({para, tryr});
//             used.insert(tryr);
//             break;
//           }
//         }
//       }
//     }
//   }
//   for (auto reg : liveness_freg->at(first)) {
//     auto para = static_cast<ParaRegister *>(reg);
//     if (spill->find(para) == spill->end()) {
//       if (para->getRegister() != nullptr &&
//           used.find(para->getRegister()) == used.end()) {
//         allocation->insert({para, para->getRegister()});
//         used.insert(para->getRegister());
//       } else {
//         while (auto tryr = Register::getFRegister(freg_cnt++)) {
//           if (used.find(tryr) != used.end()) {
//             allocation->insert({para, tryr});
//             used.insert(tryr);
//             break;
//           }
//         }
//       }
//     }
//   }

//   // allocate instruction registers
//   for (auto bb : *func->domtPreOrder) {
//     MInstruction *first = bb->getInstructions().at(0).get();
//     setUsedtoLiveIn(first, &used, allocation, liveness_ireg, liveness_freg);
//     // the way we allcate (aggrate all phi together) can increase the
//     register
//     // pressure on phi.. but make the allocation more simple
//     for (auto &phi : bb->getPhis()) {
//       Register *phyreg;
//       if (spill->find(phi.get()) == spill->end()) {
//         if (phi->getTag() == Register::V_FREGISTER) {
//           phyreg = getOneFRegiter(&used);
//         } else {
//           phyreg = getOneIRegiter(&used);
//         }
//         used.insert(phyreg);
//         allocation->insert({phi.get(), phyreg});
//       }
//     }
//     for (auto &instr : bb->getInstructions()) {
//       if (instr->getTarget() != nullptr) {
//         if (spill->find(instr->getTarget()) == spill->end()) {
//           setUsedtoLiveIn(instr.get(), &used, allocation, liveness_ireg,
//                           liveness_freg);
//           auto instrreg = instr->getTarget();
//           Register *phyreg;
//           if (instrreg->getTag() == Register::V_FREGISTER) {
//             phyreg = getOneFRegiter(&used);
//           } else {
//             phyreg = getOneIRegiter(&used);
//           }
//           allocation->insert({instrreg, phyreg});
//         }
//       }
//     }
//   }
// }

void allocate(MFunction *func, map<Register *, Register *> *allocation,
              map<Register *, int> *spill, LivenessInfo *liveness_ireg,
              LivenessInfo *liveness_freg) {
  // std::cout << "Entering allocate function." << std::endl;
  set<Register *> used;
  int ireg_cnt = 9;
  int freg_cnt = 3;
  // std::cout << "Initialized ireg_cnt to " << ireg_cnt << " and freg_cnt to "
  //           << freg_cnt << "." << std::endl;

  // allocate parameters
  // don't allocate for unused parameters
  MInstruction *first = func->getEntry()->getAllInstructions().at(0);
  // std::cout << "Allocating parameters..." << std::endl;
  for (auto reg : liveness_ireg->at(first)) {
    auto para = static_cast<ParaRegister *>(reg);
    if (spill->find(para) == spill->end()) {
      if (para->getRegister() != nullptr &&
          used.find(para->getRegister()) == used.end()) {
        allocation->insert({para, para->getRegister()});
        used.insert(para->getRegister());
        // std::cout << "Allocated parameter register: " << para->getName() <<
        // "->"
        //           << para->getRegister()->getName() << std::endl;
      } else {
        while (auto tryr = Register::getIRegister(ireg_cnt++)) {
          if (used.find(tryr) == used.end()) {
            allocation->insert({para, tryr});
            used.insert(tryr);
            // std::cout << "Allocated parameter register: " << para->getName()
            //           << "->" << tryr->getName() << std::endl;
            break;
          }
        }
      }
    }
  }

  for (auto reg : liveness_freg->at(first)) {
    auto para = static_cast<ParaRegister *>(reg);
    if (spill->find(para) == spill->end()) {
      if (para->getRegister() != nullptr &&
          used.find(para->getRegister()) == used.end()) {
        allocation->insert({para, para->getRegister()});
        used.insert(para->getRegister());
        // std::cout << "Allocated parameter register: "
        //           << para->getRegister()->getName() << std::endl;
      } else {
        while (auto tryr = Register::getFRegister(freg_cnt++)) {
          if (used.find(tryr) != used.end()) {
            allocation->insert({para, tryr});
            used.insert(tryr);
            // std::cout << "Allocated parameter register: " << tryr->getName()
            //           << std::endl;
            break;
          }
        }
      }
    }
  }

  // allocate instruction registers
  // std::cout << "Allocating instruction registers..." << std::endl;
  for (auto bb : *func->domtPreOrder) {
    MInstruction *first = bb->getAllInstructions().at(0);
    // setUsedtoLiveIn(first, &used, allocation, liveness_ireg, liveness_freg);
    // can not use setUsedtoLiveIn, beacuse phi def is also in LiveIn!
    // todo: maybe i should add a virtual parrel phi instruction...(as a super
    // instruction in normal instrucion vector)
    used.clear();
    auto iregs = liveness_ireg->at(first);
    auto fregs = liveness_freg->at(first);
    set<MHIphi*> phis;
    for (auto &phi : bb->getPhis()) {
      phis.insert(phi.get());
    }
    for (auto reg : iregs) {
      // if (reg->getTag() == Register::V_IREGISTER) {
      //   auto vreg = static_cast<VRegister*>(reg);
      //   if (vreg->isInstruction()) {
      //     auto ins = static_cast<MInstruction*>()
      //   }
      // }
      if (phis.find(static_cast<MHIphi*>(reg)) != phis.end()) {
        continue;
      }
      used.insert(allocation->at(reg));
    }
    for (auto reg : fregs) {
      if (phis.find(static_cast<MHIphi*>(reg)) != phis.end()) {
        continue;
      }
      used.insert(allocation->at(reg));
    }

    // the way we allcate (aggrate all phi together) can increase the register
    // pressure on phi.. but make the allocation more simple
    std::cout << "Allocating phi registers for basic block " << bb->getName()
              << "..." << std::endl;
    for (auto phi : phis) {
      Register *phyreg;
      if (spill->find(phi) == spill->end()) {
        if (phi->getTag() == Register::V_FREGISTER) {
          phyreg = getOneFRegiter(&used);
        } else {
          phyreg = getOneIRegiter(&used);
        }
        used.insert(phyreg);
        allocation->insert({phi, phyreg});
        std::cout << "Allocated phi register: " << phyreg->getName()
                  << std::endl;
      }
    }
    std::cout << "Allocating instruction registers for basic block "
              << bb->getName() << "..." << std::endl;
    for (auto &instr : bb->getInstructions()) {
      std::cout << "  Processing instruction: " << *instr << std::endl;

      if (instr->getTarget() != nullptr) {
        std::cout << "    Instruction has a target register: "
                  << instr->getTarget()->getName() << std::endl;

        if (spill->find(instr->getTarget()) == spill->end()) {
          std::cout << "    Target register is not in the spill set"
                    << std::endl;
          setUsedtoLiveIn(instr.get(), &used, allocation, liveness_ireg,
                          liveness_freg);
          auto instrreg = instr->getTarget();
          Register *phyreg;

          if (instrreg->getTag() == Register::V_FREGISTER) {
            std::cout << "    Target register is a floating-point register"
                      << std::endl;
            phyreg = getOneFRegiter(&used);
          } else {
            std::cout << "    Target register is an integer register"
                      << std::endl;
            phyreg = getOneIRegiter(&used);
          }

          allocation->insert({instrreg, phyreg});
          std::cout << "    Allocated instruction register: "
                    << phyreg->getName() << std::endl;
        } else {
          std::cout << "    Target register is in the spill set" << std::endl;
        }
      } else {
        std::cout << "    Instruction does not have a target register"
                  << std::endl;
      }
    }
    std::cout << "LOOP OVER" << endl;
  }

  std::cout << "Exiting allocate function." << std::endl;
}

// almost same with the one in spill_register.cc
static void out_of_ssa(MFunction *func) {
  for (auto &bb : func->getBasicBlocks()) {
    for (auto &phi : bb->getPhis()) {
      for (int i = 0; i < phi->getOprandNum(); i++) {
        auto opd = phi->getOprand(i);
        auto pre = phi->getIncomingBlock(i);
        MInstruction *ins;
        if (opd.tp == MIOprandTp::Float) {
          auto reg = opd.arg.reg;
          if (reg->getTag() == Register::V_FREGISTER) {
            ins = new MIfmv_s(reg, phi.get());
          } else {
            ins = new MImv(reg, phi.get());
          }
        } else if (opd.tp == MIOprandTp::Float) {
          auto g = func->getMod()->addGlobalFloat(new FloatConstant(opd.arg.f));
          ins = new MIflw(g, phi.get());
        } else {
          ins = new MIli(opd.arg.i, phi.get());
        }
        // If critical path
        if (pre->getOutgoings().size() > 1 && bb->getIncomings().size() > 1) {
          auto newbb =
              func->addBasicBlock("cirtical" + pre->getName() + bb->getName());
          newbb->pushInstr(ins);
          newbb->pushJmp(new MIj(&*bb));

          pre->replaceOutgoing(&*bb, newbb);
          bb->replacePhiIncoming(pre, newbb);
        } else {
          pre->pushInstr(ins);
        }
      }
    }
  }
}

// almost same with the one in spill_register.cc
static void rewrite_program_spill(MFunction *func,
                                  map<Register *, int> *spill) {
  for (auto &bb : func->getBasicBlocks()) {
    auto instrs = vector<MInstruction *>() = bb->getAllInstructions();
    for (auto ins : instrs) {
      // std::cout << "  rewrite " << *ins << endl;
      int float_cnt = 0;
      int int_cnt = 5;
      auto loads = vector<MInstruction *>();
      auto regs = std::vector<Register *>();
      for (int i = 0; i < ins->getRegNum(); i++) {
        auto reg = ins->getReg(i);
        // std::cout << "    check reg  " << reg->getName() << " " <<
        // reg->getTag()
        // << endl;
        if (spill->find(reg) != spill->end()) {
          regs.push_back(reg);
        }
      }
      std::set<Register *> s(regs.begin(), regs.end());
      regs.assign(s.begin(), s.end());
      MInstruction *load;
      for (auto reg : regs) {
        auto vreg = static_cast<VRegister *>(reg);
        if (vreg->getTag() == Register::V_FREGISTER) {
          auto target = Register::getFRegister(float_cnt++);
          int offset = spill->at(reg);
          ins->replaceRegister(reg, target);
          load = new MIflw(Register::reg_s0, -offset, target);
          loads.push_back(load);
        } else if (vreg->isPointer()) {
          auto target = Register::getIRegister(int_cnt++);
          int offset = spill->at(reg);
          ins->replaceRegister(reg, target);
          load = new MIld(Register::reg_s0, -offset, target);
          loads.push_back(load);
        } else {
          auto target = Register::getIRegister(int_cnt++);
          int offset = spill->at(reg);
          ins->replaceRegister(reg, target);
          load = new MIlw(Register::reg_s0, -offset, target);
          loads.push_back(load);
        }
        load->setComment(" Load " + reg->getName());
      }
      ins->insertBefore(loads);
      if (ins->getTarget() != nullptr &&
          (ins->getTarget()->getTag() == Register::V_FREGISTER ||
           ins->getTarget()->getTag() == Register::V_IREGISTER) &&
          (spill->find(ins->getTarget()) != spill->end())) {
        auto stores = vector<MInstruction *>();
        auto reg = ins->getTarget();
        // std::cout << "    check target  " << reg->getName() << endl;
        auto vreg = static_cast<VRegister *>(reg);
        MInstruction *store;
        if (vreg->getTag() == Register::V_FREGISTER) {
          auto target = Register::getFRegister(float_cnt++);
          int offset = spill->at(reg);
          ins->replaceRegister(reg, target);
          store = new MIfsw(target, -offset, Register::reg_s0);
          stores.push_back(store);
        } else if (vreg->isPointer()) {
          auto target = Register::getIRegister(int_cnt++);
          int offset = spill->at(reg);
          ins->replaceRegister(reg, target);
          store = new MIsd(target, -offset, Register::reg_s0);
          stores.push_back(store);
        } else {
          auto target = Register::getIRegister(int_cnt++);
          int offset = spill->at(reg);
          ins->replaceRegister(reg, target);
          store = new MIsw(target, -offset, Register::reg_s0);
          stores.push_back(store);
        }
        store->setComment(" Store " + reg->getName());
        ins->insertAfter(stores);
      }
    }
  }
}

void rewrite_program_allocate(MFunction *func,
                              map<Register *, Register *> *allocation) {
  // DEF-USE CHAIN IS BROKEN!!!
  // for (auto &[logical_reg, physical_reg] : *allocation) {
  //   logical_reg->replaceRegisterWith(physical_reg);
  // }
  for (auto &bb : func->getBasicBlocks()) {
    auto instrs = vector<MInstruction *>() = bb->getAllInstructions();
    for (auto ins : instrs) {
      auto loads = vector<MInstruction *>();
      auto regs = std::vector<Register *>();
      for (int i = 0; i < ins->getRegNum(); i++) {
        auto reg = ins->getReg(i);

        if (allocation->find(reg) != allocation->end()) {
          regs.push_back(reg);
        }
      }
      std::set<Register *> s(regs.begin(), regs.end());
      regs.assign(s.begin(), s.end());
      for (auto reg : regs) {
        ins->replaceRegister(reg, (*allocation)[reg]);
      }

      if (ins->getTarget() != nullptr &&
          (allocation->find(ins->getTarget()) != allocation->end())) {
        auto reg = ins->getTarget();
        ins->replaceRegister(reg, (*allocation)[reg]);
        ins->setComment(" ASSIGN TO " + reg->getName());
      }
    }
  }
}

// exactly same with the one in spill_register.cc
static void lower_alloca(MFunction *func, int &stack_offset) {
  for (auto &bb : func->getBasicBlocks()) {
    for (auto &ins : bb->getInstructions()) {
      if (ins->getInsTag() == MInstruction::H_ALLOCA) {
        auto alloca = static_cast<MHIalloca *>(ins.get());
        auto sz = alloca->getSize();
        stack_offset += sz;
        auto addr =
            new MIaddi(Register::reg_s0, -stack_offset, alloca->getTarget());
        ins->replaceWith({addr}); // no need to take care of use-chain after
                                  // register spill
      }
    }
  }
}

// Register Allocation on SSA Form
void allocate_register(MModule *mod) {
  for (auto &func : mod->getFunctions()) {
    // step1. Liveness Analysis
    auto liveness_ireg = make_unique<LivenessInfo>();
    auto liveness_freg = make_unique<LivenessInfo>();
    ssa_liveness_analysis(func.get(), liveness_ireg.get(), liveness_freg.get());
    std::cout << "Function: " << func->getName() << endl;

    printLivenessInfo(func.get(), liveness_ireg.get(), liveness_freg.get());

    // step2. Spill
    int offset = 0; // from sp, upward->minus
    auto spill = make_unique<map<Register *, int>>();
    spill_registers(offset, spill.get(), liveness_ireg.get(),
                    liveness_freg.get());

    std::cout << "Spill to Memory:" << endl;
    for (auto &[reg, offset] : *spill) {
      std::cout << "  " << reg->getName() << " -> " << offset << endl;
    }
    std::cout << endl;

    // // step3. Allocate
    auto allocation = make_unique<map<Register *, Register *>>();
    allocate(func.get(), allocation.get(), spill.get(), liveness_ireg.get(),
             liveness_freg.get());

    std::cout << "Register Allocation:" << endl;
    for (auto &[logical_reg, physical_reg] : *allocation) {
      std::cout << "  " << logical_reg->getName() << " -> "
                << physical_reg->getName() << endl;
    }
    std::cout << endl << endl;

    // step4. Rewrite program
    // 4.1 Remove Phi -> push move instruction to the end of the bb
    out_of_ssa(func.get());
    // 4.2 !Allocate Registers
    rewrite_program_allocate(func.get(), allocation.get());
    // 4.3 !Spill Registers
    rewrite_program_spill(func.get(), spill.get());
    // 4.4 lower alloca
    lower_alloca(func.get(), offset);
    // 4.5 Lower call instruction
    // todo: more complex beacuse we may use some arugment passing arguments...
    // 4.6 add_prelude
    // todo: more complex beacuse we should store some callee saved registes
    // 4.7 add_conclude
    // todo: more complex beacuse we should reload some callee saved registes
  }
  mod->ssa_out();
}