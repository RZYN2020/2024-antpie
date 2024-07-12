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
    std::cout << "Starting scan_back for register: " << r->getName()           \
              << std::endl;                                                    \
    for (int i = pos; i >= 0; i--) {                                           \
      auto ins = instrs[i];                                                    \
      std::cout << "  Inspecting instruction: " << *ins << std::endl;          \
      if ((*liveness)[ins].find(r) != (*liveness)[ins].end()) {                \
        std::cout << "  Register already in liveIn, returning." << std::endl;  \
        return;                                                                \
      }                                                                        \
      std::cout << "  Adding register to liveIn." << std::endl;                \
      (*liveness)[ins].insert(r);                                              \
      std::cout << "  Adding register to liveIn Over." << std::endl;           \
      auto defRegs = GET_DEFS(ins);                                            \
      for (auto defReg : defRegs) {                                            \
        if (defReg == r) {                                                     \
          std::cout << "  Register defined, returning." << std::endl;          \
          return;                                                              \
        }                                                                      \
      }                                                                        \
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
  // Initialize the LivenessInfo
  std::cout << "Starting SSA Liveness Analysis for function: "
            << func->getName() << std::endl;

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
    for (auto &phi : bb->getPhis()) {
      for (int i = 0; i < phi->getOprandNum(); i++) {
        auto pred = phi->getIncomingBlock(i);
        auto opd = phi->getOprand(i);
        if (opd.tp == MIOprandTp::Reg) {
          if (opd.arg.reg->getTag() == Register::V_IREGISTER) {
            scan_backI(static_cast<FRegister *>(opd.arg.reg), pred,
                       pred->getAllInstructions(),
                       pred->getAllInstructions().size() - 1, liveness_f);
          } else {
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
}

// Register Allocation on SSA Form
void allocate_register(MModule *mod, DomTree *dt) {
  for (auto &func : mod->getFunctions()) {
    // step1. Liveness Analysis
    auto liveness_ireg = make_unique<LivenessInfo>();
    auto liveness_freg = make_unique<LivenessInfo>();
    ssa_liveness_analysis(func.get(), liveness_ireg.get(), liveness_freg.get());
    printLivenessInfo(func.get(), liveness_ireg.get(), liveness_freg.get());

    // // step2. Spill
    // auto arg_addr = make_unique<map<Register *, int>>();
    // int offset = 0; // from sp, upward->minus
    // auto spills_ireg =
    //     make_unique<map<Register *, int>>(); // offset without considering
    //     saved
    //                                          // registers
    // auto spills_preg =
    //     make_unique<map<Register *, int>>(); // pointer register should be
    //                                          // 64-bit wise
    // auto spills_freg = make_unique<map<Register *, int>>();
    // // for all program points, if pressure is too high, spill some regsister
    // and
    // // record... until spill all

    // // step3. Allocate
    // auto used_regster =
    //     make_unique<set<Register *>>(); // which physical register all used
    // auto allocation_ireg = make_unique<map<Register *, IRegister *>>();
    // auto allocation_freg = make_unique<map<Register *, FRegister *>>();
    // travasel the dominotor tree... ignore the spilled register, allocate
    // color to other registers handle phi at the begining of the basicblock,
    // handle arguments at the begining of entry

    // step4. Rewrite program
    // 4.1 Remove Phi -> push move instruction to the end of the bb
    // 4.2 !Spill Registers
    // 4.3 !Allocate Registers
    // 4.4 lower alloca
    // 4.5 Lower call instruction
    // 4.6 add_prelude
    // 4.7 add_conclude
  }
}