#include "allocate_register.hh"

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

void out_of_ssa(MFunction *func) {
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

void rewrite_program_spill(MFunction *func, map<Register *, int> *spill) {
  for (auto &bb : func->getBasicBlocks()) {
    auto instrs = vector<MInstruction *>() = bb->getAllInstructions();
    for (auto ins : instrs) {
      int float_cnt = 0;
      int int_cnt = 5;
      auto loads = vector<MInstruction *>();
      auto regs = std::vector<Register *>();
      for (int i = 0; i < ins->getRegNum(); i++) {
        auto reg = ins->getReg(i);
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

void lower_alloca(MFunction *func, int &stack_offset) {
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

Register *getOneIRegiter(set<Register *> *used) {
  static set<Register *> all_registers = {
      Register::reg_s1, Register::reg_s2,  Register::reg_s3,  Register::reg_s4,
      Register::reg_s5, Register::reg_s6,  Register::reg_s7,  Register::reg_s8,
      Register::reg_s9, Register::reg_s10, Register::reg_s11, Register::reg_t3,
      Register::reg_t4, Register::reg_t5,  Register::reg_t6,  Register::reg_a0,
      Register::reg_a1, Register::reg_a2,  Register::reg_a3,  Register::reg_a4,
      Register::reg_a5, Register::reg_a6,  Register::reg_a7,
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

set<Register *>
getActuallCalleeSavedRegisters(map<Register *, Register *> *allocation) {
  static set<Register *> saved_registers = {
      Register::reg_gp,   Register::reg_tp,  Register::reg_s1,
      Register::reg_s2,   Register::reg_s3,  Register::reg_s4,
      Register::reg_s5,   Register::reg_s6,  Register::reg_s7,
      Register::reg_s8,   Register::reg_s9,  Register::reg_s10,
      Register::reg_s11,  Register::reg_fs0, Register::reg_fs1,
      Register::reg_fs2,  Register::reg_fs3, Register::reg_fs4,
      Register::reg_fs5,  Register::reg_fs6, Register::reg_fs7,
      Register::reg_fs8,  Register::reg_fs9, Register::reg_fs10,
      Register::reg_fs11,
  };

  set<Register *> actual_saved;
  for (auto pair : *allocation) {
    auto reg = pair.first;
    auto phyreg = allocation->at(reg);
    if (saved_registers.find(phyreg) != saved_registers.end()) {
      actual_saved.insert(phyreg);
    }
  }
  return actual_saved;
}

set<Register *>
getActuallCallerSavedRegisters(map<Register *, Register *> *allocation,
                               set<Register *> liveInOfcallsite) {
  static set<Register *> saved_registers = {
      Register::reg_t0,   Register::reg_t1,   Register::reg_t2,
      Register::reg_a0,   Register::reg_a1,   Register::reg_a2,
      Register::reg_a3,   Register::reg_a4,   Register::reg_a5,
      Register::reg_a6,   Register::reg_a7,   Register::reg_t3,
      Register::reg_t4,   Register::reg_t5,   Register::reg_t6,
      Register::reg_ft0,  Register::reg_ft1,  Register::reg_ft2,
      Register::reg_ft3,  Register::reg_ft4,  Register::reg_ft5,
      Register::reg_ft6,  Register::reg_ft7,  Register::reg_fa0,
      Register::reg_fa1,  Register::reg_fa2,  Register::reg_fa3,
      Register::reg_fa4,  Register::reg_fa5,  Register::reg_fa6,
      Register::reg_fa7,  Register::reg_ft8,  Register::reg_ft9,
      Register::reg_ft10, Register::reg_ft11,
  };
  set<Register *> intersection;
  for (auto reg : liveInOfcallsite) {
    if (allocation->find(reg) != allocation->end()) {
      auto phyreg = allocation->at(reg);
      if (saved_registers.find(reg) != saved_registers.end()) {
        intersection.insert(reg);
      }
    }
  }
  return intersection;
}

vector<MInstruction *> MHIcall::generateCallSequence(
    MFunction *func, int stack_offset, map<Register *, int> *spill,
    map<Register *, Register *> *allocation, set<Register *> *caller_saved) {

  vector<MInstruction *> push_callee_saved;
  vector<MInstruction *> pop_callee_saved;
  int stack_inc_sz = 0;
  for (auto reg : *caller_saved) {
    auto vreg = static_cast<VRegister *>(reg);
    if (vreg->isPointer()) {
      stack_inc_sz += 8;
    } else {
      stack_inc_sz += 4;
    }
    assert(allocation->find(vreg) != allocation->end());
    // maybe this piece of logic could be extrat away
    if (vreg->getTag() == V_FREGISTER) {
      push_callee_saved.push_back(
          new MIfsw(reg, -stack_inc_sz, Register::reg_sp));
      pop_callee_saved.push_back(
          new MIflw(Register::reg_sp, -stack_inc_sz, reg));
    } else if (vreg->isPointer()) {
      push_callee_saved.push_back(
          new MIsd(reg, -stack_inc_sz, Register::reg_sp));
      pop_callee_saved.push_back(
          new MIld(Register::reg_sp, -stack_inc_sz, reg));
    } else {
      push_callee_saved.push_back(
          new MIsw(reg, -stack_inc_sz, Register::reg_sp));
      pop_callee_saved.push_back(
          new MIlw(Register::reg_sp, -stack_inc_sz, reg));
    }
  }

  vector<MInstruction *> res;
  for (int i = 0; i < function->getParaSize(); i++) {
    auto funp = function->getPara(i);
    if (funp->getRegister() == nullptr) {
      stack_inc_sz += funp->getSize();
    }
  }
  int new_stack_pos = roundUp(stack_offset + stack_inc_sz, 16);

  int shift = new_stack_pos - stack_offset;
  res.push_back(new MIaddi(Register::reg_sp, -shift, Register::reg_sp));

  // load function arguments
  // conctruct map
  auto para2arg = make_unique<map<ParaRegister *, MIOprand>>();
  auto para2phyarg = make_unique<map<ParaRegister *, Register *>>();
  auto phyarg2para = make_unique<map<Register *, set<ParaRegister *>>>();

  vector<ParaRegister *> worklist;
  // set up datastructure
  for (int i = 0; i < function->getParaSize(); i++) {
    auto para = function->getPara(i);
    auto arg = args->at(i);
    if (para->getRegister() == nullptr) {
      switch (arg.tp) {
      case MIOprandTp::Float: {
        auto g = func->getMod()->addGlobalFloat(new FloatConstant(arg.arg.f));
        res.push_back(new MIflw(g, Register::reg_ft0));
        res.push_back(
            new MIfsw(Register::reg_ft0, para->getOffset(), Register::reg_sp));
      }
      case MIOprandTp::Int: {
        res.push_back(new MIli(arg.arg.i, Register::reg_t0));
        res.push_back(
            new MIsw(Register::reg_t0, para->getOffset(), Register::reg_sp));
      }
      case MIOprandTp::Reg: {
        auto reg = static_cast<VRegister *>(arg.arg.reg);
        if (reg->getTag() == Register::V_FREGISTER) {
          res.push_back(new MIfsw(reg, para->getOffset(), Register::reg_sp));
        } else if (reg->isPointer()) {
          assert(reg->getTag() == Register::V_IREGISTER);
          res.push_back(new MIsd(reg, para->getOffset(), Register::reg_sp));
        } else {
          assert(reg->getTag() == Register::V_IREGISTER);
          res.push_back(new MIsw(reg, para->getOffset(), Register::reg_sp));
        }
      }
      }
    } else {
      para2arg->insert({para, arg});
      para2phyarg->insert({para, para->getRegister()});
      if ((arg.tp == Reg) &&
          (allocation->find(arg.arg.reg) != allocation->end())) {
        auto phyarg = allocation->at(arg.arg.reg);
        if (phyarg2para->find(phyarg) == phyarg2para->end()) {
          phyarg2para->insert({phyarg, {}});
        }
        phyarg2para->at(phyarg).insert(para);
      }
      worklist.push_back(para);
    }
  }

  // todo: abstract the rearrange away
  set<Register *> handled;
  while (worklist.size() != 0) {
    auto para = *worklist.end();
    worklist.pop_back();
    if (handled.find(para) == handled.end()) {
      continue;
    }
    auto phyarg = para2phyarg->at(para); // target
    if (phyarg2para->find(phyarg) !=
        phyarg2para->end()) { // if target was used by other instr
      auto para_which_use_phyarg = phyarg2para->at(phyarg);
      if (para_which_use_phyarg.find(para) !=
          para_which_use_phyarg.end()) { // a0 = a0
        para_which_use_phyarg.erase(para);
        handled.insert(para);
        phyarg2para->erase(phyarg);
        continue;
      }
      if (para_which_use_phyarg.size() != 0) {
        if (phyarg->getTag() == I_REGISTER) {
          res.push_back(new MIfmv_s(phyarg, Register::reg_ft0));
        } else {
          res.push_back(new MImv(phyarg, Register::reg_t0));
        }
      }
      for (auto upara : para_which_use_phyarg) {
        (*para2phyarg)[para] = Register::reg_ft0;
        (*para2phyarg)[para] = Register::reg_t0;
        worklist.push_back(upara);
      }
      phyarg2para->erase(phyarg);
      continue;
    }

    auto arg = para2arg->at(para);
    switch (arg.tp) {
    case Float: {
      auto g = func->getMod()->addGlobalFloat(new FloatConstant(arg.arg.f));
      res.push_back(new MIflw(g, para->getRegister()));
    }
    case Int: {
      res.push_back(new MIli(arg.arg.i, para->getRegister()));
    }
    case Reg: {
      auto phyreg = para2phyarg->at(para);
      if (phyreg->getTag() == Register::F_REGISTER) {
        res.push_back(new MIfmv_s(phyreg, para->getRegister()));
      } else {
        assert(phyreg->getTag() == Register::V_IREGISTER);
        res.push_back(new MImv(phyreg, para->getRegister()));
      }
    }
    }
    handled.insert(para);
  }

  res.push_back(new MIcall(function));
  res.push_back(new MIaddi(Register::reg_sp, shift, Register::reg_sp));

  std::vector<MInstruction *> merged;
  merged.insert(merged.end(), push_callee_saved.begin(),
                push_callee_saved.end());
  merged.insert(merged.end(), res.begin(), res.end());
  merged.insert(merged.end(), pop_callee_saved.begin(), pop_callee_saved.end());
  return merged;
}

void add_prelude(MFunction *func, map<Register *, Register *> *allocation,
                 map<Register *, int> *spill, int stack_offset,
                 set<Register *> *callee_saved) {
  int stack_zs = stack_offset;
  auto entry = func->getEntry();
  auto sub_sp = new MIaddi(Register::reg_sp, -stack_zs, Register::reg_sp);
  auto store_ra = new MIsd(Register::reg_ra, stack_zs - 8, Register::reg_sp);
  auto store_fp = new MIsd(Register::reg_s0, stack_zs - 16, Register::reg_sp);
  auto get_fp = new MIaddi(Register::reg_sp, stack_zs, Register::reg_s0);
  auto prelude = vector<MInstruction *>({sub_sp, store_ra, store_fp, get_fp});

  int offset = 16;
  for (auto reg : *callee_saved) {
    offset += 8;
    if (reg->getTag() == Register::I_REGISTER) {
      prelude.push_back(new MIsd(reg, -offset, Register::reg_s0));
    } else {
      prelude.push_back(new MIfsd(reg, -offset, Register::reg_s0));
    }
  }

  // load function arguments
  // conctruct map
  auto para2phyarg = make_unique<map<ParaRegister *, Register *>>();
  auto phyarg2para = make_unique<map<Register *, set<ParaRegister *>>>();

  vector<ParaRegister *> worklist;
  // set up datastructure
  for (int i = 0; i < func->getParaSize(); i++) {
    auto para = func->getPara(i);
    if (spill->find(para) != spill->end()) {
      auto addr = spill->at(para);
      if (para->getTag() == Register::V_FREGISTER) {
        prelude.push_back(new MIfsw(para, -addr, Register::reg_s0));
      } else if (para->isPointer()) {
        prelude.push_back(new MIsd(para, -addr, Register::reg_s0));
      } else {
        prelude.push_back(new MIsw(para, -addr, Register::reg_s0));
      }
    } else {
      para2phyarg->insert({para, para->getRegister()});
      auto phyarg = allocation->at(para);
      if (phyarg2para->find(phyarg) == phyarg2para->end()) {
        phyarg2para->insert({phyarg, {}});
      }
      phyarg2para->at(phyarg).insert(para);
      worklist.push_back(para);
    }
  }

  // todo: abstract the rearrange away
  set<Register *> handled;
  while (worklist.size() != 0) {
    auto para = *worklist.end();
    worklist.pop_back();
    if (handled.find(para) == handled.end()) {
      continue;
    }
    auto phyarg = para2phyarg->at(para); // target
    if (phyarg2para->find(phyarg) !=
        phyarg2para->end()) { // if target was used by other instr
      auto para_which_use_phyarg = phyarg2para->at(phyarg);
      if (para_which_use_phyarg.find(para) !=
          para_which_use_phyarg.end()) { // a0 = a0
        para_which_use_phyarg.erase(para);
        handled.insert(para);
        phyarg2para->erase(phyarg);
        continue;
      }
      if (para_which_use_phyarg.size() != 0) {
        if (phyarg->getTag() == Register::I_REGISTER) {
          prelude.push_back(new MIfmv_s(phyarg, Register::reg_ft0));
        } else {
          prelude.push_back(new MImv(phyarg, Register::reg_t0));
        }
      }
      for (auto upara : para_which_use_phyarg) {
        (*para2phyarg)[para] = Register::reg_ft0;
        (*para2phyarg)[para] = Register::reg_t0;
        worklist.push_back(upara);
      }
      phyarg2para->erase(phyarg);
      continue;
    }

    auto phyreg = para2phyarg->at(para);
    if (phyreg->getTag() == Register::F_REGISTER) {
      prelude.push_back(new MIfmv_s(phyreg, para->getRegister()));
    } else {
      assert(phyreg->getTag() == Register::V_IREGISTER);
      prelude.push_back(new MImv(phyreg, para->getRegister()));
    }
    handled.insert(para);
  }
  entry->pushInstrsAtHead(prelude);
}

void add_conclude(MFunction *func, map<Register *, Register *> *allocation,
                  map<Register *, int> *spill, int stack_offset,
                  set<Register *> *callee_saved) {

  for (auto &bb : func->getBasicBlocks()) {
    for (auto &jmp : bb->getJmps()) {
      if (jmp->getInsTag() == MInstruction::H_RET) {
        auto hret = static_cast<MHIret *>(jmp.get());
        switch (hret->r.tp) {
        case MIOprandTp::Float: {
          auto g =
              func->getMod()->addGlobalFloat(new FloatConstant(hret->r.arg.f));
          bb->pushInstr(new MIflw(g, Register::reg_fa0));
          break;
        }
        case MIOprandTp::Int: {
          bb->pushInstr(new MIli(hret->r.arg.i, Register::reg_a0));
          break;
        }
        case MIOprandTp::Reg: {
          auto reg = hret->r.arg.reg;
          if (allocation->find(reg) != allocation->end()) {
            auto phyreg = allocation->at(reg);
            if (reg->getTag() == Register::F_REGISTER) {
              bb->pushInstr(new MIfmv_s(phyreg, Register::reg_fa0));
            } else {
              bb->pushInstr(new MImv(phyreg, Register::reg_a0));
            }
          } else {
            auto addr = spill->at(reg);
            if (reg->getTag() == Register::F_REGISTER) {
              bb->pushInstr(
                  new MIflw(Register::reg_s0, -addr, Register::reg_fa0));
            } else {
              bb->pushInstr(
                  new MIlw(Register::reg_s0, -addr, Register::reg_a0));
            }
          }
          break;
        }
        }
        hret->replaceWith({new MIret()});
      }
    }
  }

  auto exit = func->addBasicBlock(func->getName() + "_exit");
  int offset = 16;
  for (auto reg : *callee_saved) {
    offset += 8;
    if (reg->getTag() == Register::I_REGISTER) {
      exit->pushInstr(new MIld(Register::reg_s0, -offset, reg));
    } else {
      exit->pushInstr(new MIfld(Register::reg_s0, -offset, reg));
    }
  }

  int stack_sz = stack_offset;
  auto load_ra = new MIld(Register::reg_sp, stack_sz - 8, Register::reg_ra);
  auto load_fp = new MIld(Register::reg_sp, stack_sz - 16, Register::reg_s0);
  auto add_sp = new MIaddi(Register::reg_sp, stack_sz, Register::reg_sp);
  auto ret = new MIret();
  exit->pushInstrs({load_ra, load_fp, add_sp, ret});

  for (auto &bb : func->getBasicBlocks()) {
    if (bb.get() == exit)
      continue;
    for (auto &i : bb->getJmps()) {
      if (i->getInsTag() == MInstruction::RET) {
        i->replaceWith({new MIj(exit)});
      }
    }
  }
}