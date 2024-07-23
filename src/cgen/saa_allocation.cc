#include "DomTree.hh"
#include "allocate_register.hh"
#include <set>

///////////////////////////////////////
///////////////////////////////////////
/////////  Liveness Analysis
///////////////////////////////////////
///////////////////////////////////////

static void scan_back(Register *r, MBasicBlock *bb,
                      vector<MInstruction *> instrs, int pos,
                      map<MInstruction *, set<Register *>> *liveness,
                      GET_DEFS get_defs, GET_PHIS get_phis) {
  for (int i = pos; i >= 0; i--) {
    auto ins = instrs[i];
    if ((*liveness)[ins].find(r) != (*liveness)[ins].end()) {
      return;
    }
    vector<Register *> defRegs = get_defs(ins);
    for (auto defReg : defRegs) {
      if (defReg == r) {
        return;
      }
    }
    (*liveness)[ins].insert(r);
  }
  vector<Register *> phiDefs = get_phis(bb);
  for (auto phiDef : phiDefs) {
    if (phiDef == r) {
      return;
    }
  }
  for (auto pred : bb->getIncomings()) {
    scan_back(r, pred, pred->getAllInstructions(),
              pred->getAllInstructions().size() - 1, liveness, get_defs,
              get_phis);
  }
}

// todo: two pass liveness analysis:
// https://inria.hal.science/inria-00558509v2/document
// https://dl.acm.org/doi/abs/10.1007/978-3-319-07959-2_15
// We use a simple Path Exploration Algorithm currently
void ssa_liveness_analysis(MFunction *func, LivenessInfo *liveness_i,
                           LivenessInfo *liveness_f) {
#define VI Register::V_IREGISTER
#define VF Register::V_FREGISTER
  for (auto bb : func->getBasicBlocks()) {
    auto instrs = bb->getAllInstructions();
    for (auto ins : instrs) {
      (*liveness_i)[ins] = set<Register *>();
      (*liveness_f)[ins] = set<Register *>();
    }
  }

  for (auto bb : func->getBasicBlocks()) {
    auto instrs = bb->getAllInstructions(); // without phi
    for (int i = static_cast<int>(instrs.size()) - 1; i >= 0; i--) {
      auto ins = instrs[i];
      for (auto r : getUses<VI>(ins)) {
        scan_back(r, bb, instrs, i, liveness_i, getDefs<VI>, getPhiDefs<VI>);
      }
      for (auto r : getUses<VF>(ins)) {
        scan_back(r, bb, instrs, i, liveness_f, getDefs<VF>, getPhiDefs<VF>);
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
            scan_back(static_cast<FRegister *>(opd.arg.reg), pred,
                      pred->getAllInstructions(),
                      pred->getAllInstructions().size() - 1, liveness_f,
                      getDefs<VF>, getPhiDefs<VF>);
          } else {
            (*liveness_f)[first].insert(opd.arg.reg);
            scan_back(static_cast<IRegister *>(opd.arg.reg), pred,
                      pred->getAllInstructions(),
                      pred->getAllInstructions().size() - 1, liveness_i,
                      getDefs<VI>, getPhiDefs<VI>);
          }
        }
      }
    }
  }
}

///////////////////////////////////////
///////////////////////////////////////
/////////  spill Register
///////////////////////////////////////
///////////////////////////////////////

template <int MAX_REG_NUM>
static void spillRegisters(std::map<Register *, int> *spilled,
                           LivenessInfo *liveness, int &stackOffset) {
  for (auto &pair : *liveness) {
    std::vector<Register *> regs;
    for (auto reg : pair.second) {
      if (spilled->find(reg) == spilled->end()) {
        regs.push_back(reg);
      }
    }
    while (regs.size() >= MAX_REG_NUM) {
      auto reg = static_cast<VRegister *>(regs.back());
      regs.pop_back();
      if (!reg->isInstruction()) {
        auto para = static_cast<ParaRegister *>(reg);
        (*spilled)[reg] = -para->getOffset();
      } else {
        if (reg->isPointer()) {
          stackOffset += 8;
        } else {
          stackOffset += 4;
        }
        (*spilled)[reg] = stackOffset;
      }
    }
  }
}

static void spill_registers(int &stk_offset, std::map<Register *, int> *spilled,
                            LivenessInfo *liveness_ireg,
                            LivenessInfo *liveness_freg) {
  spillRegisters<MAX_I_REG_NUM>(spilled, liveness_ireg, stk_offset);
  spillRegisters<MAX_F_REG_NUM>(spilled, liveness_freg, stk_offset);
}

static void setUsedtoLiveIn(MInstruction *ins, set<Register *> *used,
                            map<Register *, Register *> *allocation,
                            LivenessInfo *liveness_ireg,
                            LivenessInfo *liveness_freg) {
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

///////////////////////////////////////
///////////////////////////////////////
/////////  Allocate Register
///////////////////////////////////////
///////////////////////////////////////

static void allocateParaRegister(ParaRegister *para,
                                 map<Register *, Register *> *allocation,
                                 set<Register *> &used, int &ireg_cnt,
                                 int &freg_cnt) {
  if (para->getRegister() != nullptr &&
      used.find(para->getRegister()) == used.end()) {
    allocation->insert({para, para->getRegister()});
    used.insert(para->getRegister());
  } else {
    while (auto tryr = (para->getTag() == Register::V_FREGISTER)
                           ? Register::getFRegister(freg_cnt++)
                           : Register::getIRegister(ireg_cnt++)) {
      if (used.find(tryr) == used.end()) {
        allocation->insert({para, tryr});
        used.insert(tryr);
        break;
      }
    }
  }
}

static void allocateParameters(MFunction *func,
                               map<Register *, Register *> *allocation,
                               map<Register *, int> *spill,
                               LivenessInfo *liveness_ireg,
                               LivenessInfo *liveness_freg) {
  set<Register *> used;
  int ireg_cnt = 9;
  int freg_cnt = 3;

  // don't allocate for unused parameters
  for (auto reg :
       liveness_ireg->at(func->getEntry()->getAllInstructions().at(0))) {
    auto para = static_cast<ParaRegister *>(reg);
    if (spill->find(para) == spill->end()) {
      allocateParaRegister(para, allocation, used, ireg_cnt, freg_cnt);
    }
  }

  for (auto reg :
       liveness_freg->at(func->getEntry()->getAllInstructions().at(0))) {
    auto para = static_cast<ParaRegister *>(reg);
    if (spill->find(para) == spill->end()) {
      allocateParaRegister(para, allocation, used, ireg_cnt, freg_cnt);
    }
  }
}

void allocatTheRegister(Register *reg, map<Register *, Register *> *allocation,
                        set<Register *> &used) {
  Register *phyreg;
  if (reg->getTag() == Register::V_FREGISTER) {
    phyreg = getOneFRegiter(&used);
  } else {
    phyreg = getOneIRegiter(&used);
  }
  used.insert(phyreg);
  allocation->insert({reg, phyreg});
}

// the way we allcate (aggrate all phi together) can increase the register
// pressure on phi.. but make the allocation more simple
void allocatePhis(MBasicBlock *bb, map<Register *, Register *> *allocation,
                  LivenessInfo *liveness_ireg, LivenessInfo *liveness_freg,
                  map<Register *, int> *spill) {
  set<Register *> used;
  set<MHIphi *> phis;
  for (auto phi : bb->getPhis()) {
    phis.insert(phi);
  }

  for (auto reg : liveness_ireg->at(bb->getAllInstructions().at(0))) {
    if (phis.find(static_cast<MHIphi *>(reg)) != phis.end() &&
        (spill->find(reg) == spill->end())) {
      allocatTheRegister(reg, allocation, used);
    }
  }
  for (auto reg : liveness_freg->at(bb->getAllInstructions().at(0))) {
    if (phis.find(static_cast<MHIphi *>(reg)) != phis.end() &&
        (spill->find(reg) == spill->end())) {
      allocatTheRegister(reg, allocation, used);
    }
  }
}

// without phi
void allocateOtherInstruction(MBasicBlock *bb,
                              map<Register *, Register *> *allocation,
                              LivenessInfo *liveness_ireg,
                              LivenessInfo *liveness_freg,
                              map<Register *, int> *spill) {
  // setUsedtoLiveIn(first, &used, allocation, liveness_ireg, liveness_freg);
  // can not use setUsedtoLiveIn, beacuse phi def is also in LiveIn!
  // todo: maybe i should add a virtual parrel phi instruction...(as a super
  // instruction in normal instrucion vector)
  set<Register *> used;
  for (auto instr : bb->getInstructions()) {
    if (instr->getTarget() != nullptr &&
        spill->find(instr->getTarget()) == spill->end()) {
      setUsedtoLiveIn(instr, &used, allocation, liveness_ireg, liveness_freg);
      allocatTheRegister(instr, allocation, used);
    }
  }
}

void allocateInstructions(MFunction *func,
                          map<Register *, Register *> *allocation,
                          map<Register *, int> *spill,
                          LivenessInfo *liveness_ireg,
                          LivenessInfo *liveness_freg) {

  for (auto bb : *func->domtPreOrder) {
    allocatePhis(bb, allocation, liveness_ireg, liveness_freg, spill);
    allocateOtherInstruction(bb, allocation, liveness_ireg, liveness_freg,
                             spill);
  }
}

static void allocate(MFunction *func, map<Register *, Register *> *allocation,
                     map<Register *, int> *spill, LivenessInfo *liveness_ireg,
                     LivenessInfo *liveness_freg) {
  allocateParameters(func, allocation, spill, liveness_ireg, liveness_ireg);
  allocateInstructions(func, allocation, spill, liveness_ireg, liveness_ireg);
}

///////////////////////////////////////
///////////////////////////////////////
/////////   Main Entry
///////////////////////////////////////
///////////////////////////////////////

// Register Allocation on SSA Form
void allocate_register(MModule *mod) {
  for (auto func : mod->getFunctions()) {
    // step1. Liveness Analysis
    auto liveness_ireg = make_unique<LivenessInfo>();
    auto liveness_freg = make_unique<LivenessInfo>();
    ssa_liveness_analysis(func, liveness_ireg.get(), liveness_freg.get());
    std::cout << "Function: " << func->getName() << endl;

    // printLivenessInfo(func.get(), liveness_ireg.get(), liveness_freg.get());

    // step2. Spill
    int offset = 16; // from fp, upward->minus
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
    allocate(func, allocation.get(), spill.get(), liveness_ireg.get(),
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
    out_of_ssa(func);
    // std::cout << "endl" << endl;
    lower_call(func, offset, allocation.get(), spill.get(), liveness_ireg.get(),
               liveness_ireg.get());
    lower_alloca(func, offset);
    add_prelude(func, allocation.get(), spill.get(), offset, &callee_saved);
    add_conclude(func, allocation.get(), spill.get(), offset, &callee_saved);
    rewrite_program_allocate(func, allocation.get());
    rewrite_program_spill(func, spill.get());
    fixRange(func);
  }
  mod->ssa_out();
}