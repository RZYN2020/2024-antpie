#include "DomTree.hh"
#include "allocate_register.hh"
#include <set>

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
    set<MHIphi *> phis;
    for (auto &phi : bb->getPhis()) {
      phis.insert(phi.get());
    }
    for (auto reg : iregs) {
      if (phis.find(static_cast<MHIphi *>(reg)) != phis.end()) {
        continue;
      }
      used.insert(allocation->at(reg));
    }
    for (auto reg : fregs) {
      if (phis.find(static_cast<MHIphi *>(reg)) != phis.end()) {
        continue;
      }
      used.insert(allocation->at(reg));
    }

    // the way we allcate (aggrate all phi together) can increase the register
    // pressure on phi.. but make the allocation more simple

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
      }
    }
    for (auto &instr : bb->getInstructions()) {
      if (instr->getTarget() != nullptr) {
        if (spill->find(instr->getTarget()) == spill->end()) {
          setUsedtoLiveIn(instr.get(), &used, allocation, liveness_ireg,
                          liveness_freg);
          auto instrreg = instr->getTarget();
          Register *phyreg;
          if (instrreg->getTag() == Register::V_FREGISTER) {
            phyreg = getOneFRegiter(&used);
          } else {
            phyreg = getOneIRegiter(&used);
          }
          allocation->insert({instrreg, phyreg});
        }
      }
    }
  }
}

// exactly same with the one in spill_register.cc

// Register Allocation on SSA Form
void allocate_register(MModule *mod) {
  for (auto &func : mod->getFunctions()) {
    // step1. Liveness Analysis
    auto liveness_ireg = make_unique<LivenessInfo>();
    auto liveness_freg = make_unique<LivenessInfo>();
    ssa_liveness_analysis(func.get(), liveness_ireg.get(), liveness_freg.get());
    // std::cout << "Function: " << func->getName() << endl;

    // printLivenessInfo(func.get(), liveness_ireg.get(), liveness_freg.get());

    // step2. Spill
    int offset = 0; // from sp, upward->minus
    auto spill = make_unique<map<Register *, int>>();
    spill_registers(offset, spill.get(), liveness_ireg.get(),
                    liveness_freg.get());

    // std::cout << "Spill to Memory:" << endl;
    // for (auto &[reg, offset] : *spill) {
    //   std::cout << "  " << reg->getName() << " -> " << offset << endl;
    // }
    // std::cout << endl;

    // // step3. Allocate
    auto allocation = make_unique<map<Register *, Register *>>();
    allocate(func.get(), allocation.get(), spill.get(), liveness_ireg.get(),
             liveness_freg.get());

    // std::cout << "Register Allocation:" << endl;
    // for (auto &[logical_reg, physical_reg] : *allocation) {
    //   std::cout << "  " << logical_reg->getName() << " -> "
    //             << physical_reg->getName() << endl;
    // }
    // std::cout << endl << endl;

    auto callee_saved = getActuallCalleeSavedRegisters(allocation.get());
    offset += callee_saved.size() * 8;

    // step4. Rewrite program
    out_of_ssa(func.get());
    lower_alloca(func.get(), offset);
    add_prelude(func.get(), allocation.get(), spill.get(), offset,
                &callee_saved);
    add_conclude(func.get(), allocation.get(), spill.get(), offset,
                 &callee_saved);
    rewrite_program_allocate(func.get(), allocation.get());
    rewrite_program_spill(func.get(), spill.get());
  }
  mod->ssa_out();
}