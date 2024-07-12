#include "Machine.hh"
#include <set>

static void out_of_ssa(MModule *mod) {
  for (auto &func : mod->getFunctions()) {
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
            auto g =
                func->getMod()->addGlobalFloat(new FloatConstant(opd.arg.f));
            ins = new MIflw(g, phi.get());
          } else {
            ins = new MIli(opd.arg.i, phi.get());
          }
          // If critical path
          if (pre->getOutgoings().size() > 1 && bb->getIncomings().size() > 1) {
            auto newbb = func->addBasicBlock("cirtical" + pre->getName() +
                                             bb->getName());
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
  mod->ssa_out();
}

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

// https://stackoverflow.com/questions/3407012/rounding-up-to-the-nearest-multiple-of-a-number
static int roundUp(int numToRound, int multiple) {
  assert(multiple && ((multiple & (multiple - 1)) == 0));
  return (numToRound + multiple - 1) & -multiple;
}

vector<MInstruction *>
MHIcall::generateCallSequence(MFunction *func, int stack_offset,
                              map<Register *, int> *spill) {

  int stack_arg_sz = 0;
  vector<MInstruction *> res;
  for (int i = 0; i < function->getParaSize(); i++) {
    auto funp = function->getPara(i);
    if (funp->getRegister() == nullptr) {
      stack_arg_sz += funp->getSize();
    }
  }
  int new_stack_pos = roundUp(stack_offset + stack_arg_sz, 16);
  int shift = new_stack_pos - stack_offset;
  res.push_back(new MIaddi(Register::reg_sp, -shift, Register::reg_sp));
  for (int i = 0; i < function->getParaSize(); i++) {
    auto funp = function->getPara(i);     // represent the target
    auto arg = args->at(i);               // represent the source
    if (funp->getRegister() == nullptr) { // store to stack
      switch (arg.tp) {
      case MIOprandTp::Float: {
        auto g = func->getMod()->addGlobalFloat(new FloatConstant(arg.arg.f));
        res.push_back(new MIflw(g, Register::reg_ft0));
        res.push_back(
            new MIfsw(Register::reg_ft0, funp->getOffset(), Register::reg_sp));
      }
      case MIOprandTp::Int: {
        res.push_back(new MIli(arg.arg.i, Register::reg_t0));
        res.push_back(
            new MIsw(Register::reg_t0, funp->getOffset(), Register::reg_sp));
      }
      case MIOprandTp::Reg: {
        auto reg = static_cast<VRegister *>(arg.arg.reg);
        auto addr =
            spill->at(reg); //  not nessery... it can be down by rewriting
        if (reg->getTag() == Register::V_FREGISTER) {
          res.push_back(new MIflw(Register::reg_s0, -addr, Register::reg_ft0));
          res.push_back(new MIfsw(Register::reg_ft0, funp->getOffset(),
                                  Register::reg_sp));
        } else if (reg->isPointer()) {
          assert(reg->getTag() == Register::V_IREGISTER);
          res.push_back(new MIld(Register::reg_s0, -addr, Register::reg_t0));
          res.push_back(
              new MIsd(Register::reg_t0, funp->getOffset(), Register::reg_sp));
        } else {
          assert(reg->getTag() == Register::V_IREGISTER);
          res.push_back(new MIlw(Register::reg_s0, -addr, Register::reg_t0));
          res.push_back(
              new MIsw(Register::reg_t0, funp->getOffset(), Register::reg_sp));
        }
      }
      }
    } else { // move to register
      auto reg = funp->getRegister();
      switch (arg.tp) {
      case Float: {
        auto g = func->getMod()->addGlobalFloat(new FloatConstant(arg.arg.f));
        res.push_back(new MIflw(g, reg));
      }
      case Int: {
        res.push_back(new MIli(arg.arg.i, reg));
      }
      case Reg: {
        auto reg = static_cast<VRegister *>(arg.arg.reg);
        auto addr = spill->at(reg);
        if (reg->getTag() == Register::V_FREGISTER) {
          res.push_back(new MIflw(Register::reg_s0, -addr, reg));
        } else if (reg->isPointer()) {
          assert(reg->getTag() == Register::V_IREGISTER);
          res.push_back(new MIld(Register::reg_s0, -addr, reg));
        } else {
          assert(reg->getTag() == Register::V_IREGISTER);
          res.push_back(new MIlw(Register::reg_s0, -addr, reg));
        }
      }
      }
    }
  }
  res.push_back(new MIcall(function));
  res.push_back(new MIaddi(Register::reg_sp, shift, Register::reg_sp));
  return res;
}

static void lower_call(MFunction *func, map<Register *, int> *spill,
                       int &stack_offset) {
  for (auto &bb : func->getBasicBlocks()) {
    for (auto &ins : bb->getInstructions()) {
      if (ins->getInsTag() == MInstruction::H_CALL) {
        auto call = static_cast<MHIcall *>(ins.get());
        ins->replaceWith(call->generateCallSequence(func, stack_offset, spill));
      }
    }
  }
}

static void add_prelude(MFunction *func, map<Register *, int> *spill,
                        int stack_offset) {
  int stack_zs = stack_offset;
  auto entry = func->getEntry();
  auto sub_sp = new MIaddi(Register::reg_sp, -stack_zs, Register::reg_sp);
  auto store_ra = new MIsd(Register::reg_ra, stack_zs - 8, Register::reg_sp);
  auto store_fp = new MIsd(Register::reg_s0, stack_zs - 16, Register::reg_sp);
  auto get_fp = new MIaddi(Register::reg_sp, stack_zs, Register::reg_s0);
  auto prelude = vector<MInstruction *>({sub_sp, store_ra, store_fp, get_fp});

  for (int i = 0; i < func->getParaSize(); i++) {
    auto funp = func->getPara(i); // represent the target
    if (funp->getRegister() != nullptr) {
      auto addr = spill->at(funp);
      if (funp->getTag() == Register::V_FREGISTER) {
        prelude.push_back(
            new MIfsw(funp->getRegister(), -addr, Register::reg_s0));
      } else if (funp->isPointer()) {
        prelude.push_back(
            new MIsd(funp->getRegister(), -addr, Register::reg_s0));
      } else {
        prelude.push_back(
            new MIsw(funp->getRegister(), -addr, Register::reg_s0));
      }
    }
  }

  entry->pushInstrsAtHead(prelude);
}

static void add_conclude(MFunction *func, map<Register *, int> *spill,
                         int stack_offset) {

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
          auto addr = spill->at(reg);
          if (reg->getTag() == Register::F_REGISTER) {
            bb->pushInstr(
                new MIflw(Register::reg_s0, -addr, Register::reg_fa0));
          } else {
            bb->pushInstr(new MIlw(Register::reg_s0, -addr, Register::reg_a0));
          }
          break;
        }
        }
        hret->replaceWith({new MIret()});
      }
    }
  }

  int stack_sz = stack_offset;
  auto exit = func->addBasicBlock(func->getName() + "_exit");
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

static void rewrite_program(MFunction *func, map<Register *, int> *spill) {
  for (auto &bb : func->getBasicBlocks()) {
    auto instrs = vector<MInstruction *>();
    for (auto &ins : bb->getInstructions()) {
      instrs.push_back(ins.get());
    }
    for (auto &ins : bb->getJmps()) {
      instrs.push_back(ins.get());
    }
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
        if (reg->getTag() == Register::V_FREGISTER ||
            reg->getTag() == Register::V_IREGISTER) {
          // std::cout << "      need replace" << endl;
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
           ins->getTarget()->getTag() == Register::V_IREGISTER)) {
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
  rewrite_program(func, spill.get());
  lower_alloca(func, stack_offset);
  lower_call(func, spill.get(), stack_offset);
  add_prelude(func, spill.get(), stack_offset);
  add_conclude(func, spill.get(), stack_offset);
}

void spill_all_register(MModule *mod) {
  // step1. out-of-ssa

  out_of_ssa(mod);
  mod->ssa_out();
  // step2: spill registers
  for (auto &func : mod->getFunctions()) {
    spill_register_for_func(func.get());
  }
}
