#include "Machine.hh"
#include <set>

enum MTP {
  FLOAT,
  POINTER,
  INT,
};

MTP getType(Register *reg) {
  if (reg->getTag() == Register::A_REGISTER) {
    auto arg = static_cast<ARGRegister *>(reg);
    if (arg->getIRArgument()->getType()->getTypeTag() == TT_POINTER) {
      return POINTER;
    } else if (arg->getIRArgument()->getType()->getTypeTag() == TT_FLOAT) {
      return FLOAT;
    } else {
      return INT;
    }
  } else {
    auto ins = static_cast<MInstruction *>(reg);
    switch (ins->getInsTag()) {
    case MInstruction::H_ALLOCA:
    case MInstruction::ADD:
    case MInstruction::ADDI:
    case MInstruction::MUL: { // only generate by gep and alloca
      return POINTER;
    }
    case MInstruction::FADD_S:
    case MInstruction::FSUB_S:
    case MInstruction::FMUL_S:
    case MInstruction::FDIV_S:
    case MInstruction::FMV_S:
    case MInstruction::FLW:
    case MInstruction::FCVTS_W: {
      return FLOAT;
    }
    default: {
      return INT;
    }
    }
  }
}

void out_of_ssa(MModule *mod) {
  for (auto &func : mod->getFunctions()) {
    for (auto &bb : func->getBasicBlocks()) {
      for (auto &phi : bb->getPhis()) {
        for (int i = 0; i < phi->getRegNum(); i++) {
          auto reg = phi->getReg(i);
          auto pre = phi->getIncomingBlock(i);
          MInstruction *move;
          if (getType(reg) == FLOAT) {
            move = new MIfmv_s(reg, phi.get());
          } else {
            move = new MImv(reg, phi.get());
          }
          // If critical path
          if (pre->getOutgoings().size() > 1 && bb->getIncomings().size() > 1) {
            auto newbb = func->addBasicBlock("cirtical" + pre->getName() +
                                             bb->getName());
            newbb->pushInstr(move);
            newbb->pushJmp(new MIj(&*bb));

            pre->replaceOutgoing(&*bb, newbb);
            bb->replacePhiIncoming(pre, newbb);
          } else {
            pre->pushInstr(move);
          }
        }
      }
    }
  }
  mod->ssa_out();
}

#define SWITCH_ON_TP()                                                         \
  switch (tp) {                                                                \
  case POINTER: {                                                              \
    stack_offset += 8;                                                         \
    tp_map->insert({reg, POINTER});                                            \
    break;                                                                     \
  }                                                                            \
  case FLOAT: {                                                                \
    stack_offset += 4;                                                         \
    tp_map->insert({reg, FLOAT});                                              \
    break;                                                                     \
  }                                                                            \
  case INT: {                                                                  \
    stack_offset += 4;                                                         \
    tp_map->insert({reg, INT});                                                \
    break;                                                                     \
  }                                                                            \
  }                                                                            \
  allocation->insert({reg, stack_offset})

void allocate_arguments(MFunction *func, map<Register *, uint32_t> *allocation,
                        map<Register *, MTP> *tp_map, uint32_t &stack_offset) {
  // allocate arguments
  auto ftp = func->getType();
  for (int i = 0; i < func->getType()->getArgSize(); i++) {
    auto argument = func->getArg(i);
    auto reg = argument;
    auto tp = getType(argument);
    SWITCH_ON_TP();
  }
}

void allocate_registers(MFunction *func, map<Register *, uint32_t> *allocation,
                        map<Register *, MTP> *tp_map, uint32_t &stack_offset) {
  // allocate registers
  for (auto &bb : func->getBasicBlocks()) {
    for (auto &ins : bb->getPhis()) {
      auto arg = ins->getReg(0);
      auto tp = getType(arg);
      auto reg = ins.get();
      SWITCH_ON_TP();
    }
    for (auto &ins : bb->getInstructions()) {
      if (ins->getTarget() != ins.get()) { // not used as register
        continue;
      }
      auto tp = getType(ins.get());
      auto reg = ins.get();
      SWITCH_ON_TP();
    }
  }
}

void rewrite_program(MFunction *func, map<Register *, uint32_t> *allocation,
                     map<Register *, MTP> *tp_map) {
  for (auto &bb : func->getBasicBlocks()) {
    auto instrs = vector<MInstruction *>();
    for (auto &ins : bb->getInstructions()) {
      instrs.push_back(ins.get());
    }
    for (auto &ins : bb->getJmps()) {
      instrs.push_back(ins.get());
    }
    for (auto ins : instrs) {
      int float_cnt = 0;
      int int_cnt = 5;
      auto loads = vector<MInstruction *>();

      auto regs = std::vector<Register *>();
      for (int i = 0; i < ins->getRegNum(); i++) {
        regs.push_back(ins->getReg(i));
      }
      std::set<Register *> s(regs.begin(), regs.end());
      regs.assign(s.begin(), s.end());

      MInstruction *load;
      for (auto reg : regs) {
        switch (tp_map->at(reg)) {
        case FLOAT: {
          auto target = Register::getFRegister(float_cnt++);
          int offset = allocation->at(reg);
          ins->replaceRegister(reg, target);
          load = new MIflw(Register::reg_s0, -offset, target);
          loads.push_back(load);
          break;
        }
        case POINTER: {
          auto target = Register::getIRegister(int_cnt++);
          int offset = allocation->at(reg);
          ins->replaceRegister(reg, target);
          load = new MIld(Register::reg_s0, -offset, target);
          loads.push_back(load);
          break;
        }
        case INT: {
          auto target = Register::getIRegister(int_cnt++);
          int offset = allocation->at(reg);
          ins->replaceRegister(reg, target);
          load = new MIlw(Register::reg_s0, -offset, target);
          loads.push_back(load);
          break;
        }
        }
        load->setComment(" Load " + reg->getName());
      }

      ins->insertBefore(loads);

      if (ins->getTarget() != nullptr) {
        auto stores = vector<MInstruction *>();
        auto reg = ins->getTarget();
        MInstruction *store;
        switch (tp_map->at(reg)) {
        case FLOAT: {
          auto target = Register::getFRegister(float_cnt++);
          int offset = allocation->at(reg);
          ins->replaceRegister(reg, target);
          store = new MIfsw(target, -offset, Register::reg_s0);
          stores.push_back(store);
          break;
        }
        case POINTER: {
          auto target = Register::getIRegister(int_cnt++);
          int offset = allocation->at(reg);
          ins->replaceRegister(reg, target);
          store = new MIsd(target, -offset, Register::reg_s0);
          stores.push_back(store);
          break;
        }
        case INT: {
          auto target = Register::getIRegister(int_cnt++);
          int offset = allocation->at(reg);
          ins->replaceRegister(reg, target);
          store = new MIsw(target, -offset, Register::reg_s0);
          stores.push_back(store);
          break;
        }
        default:
          break;
        }
        store->setComment(" Store " + reg->getName());
        ins->insertAfter(stores);
      }
    }
  }
}

void lower_hinstrs(MFunction *func, map<Register *, uint32_t> *allocation,
                   map<Register *, MTP> *tp_map) {
  for (auto &bb : func->getBasicBlocks()) {
    for (auto &ins : bb->getInstructions()) {
      if (ins->getInsTag() == MInstruction::H_ALLOCA) {
        auto alloca = static_cast<MHIalloca *>(ins.get());
        auto sz = alloca->getSize();
        func->stack_offset += sz;
        auto addr = new MIaddi(Register::reg_s0, -func->stack_offset,
                               alloca->getTarget());
        ins->replaceWith({addr}); // no need to take care of use-chain after
                                  // register allocation
      }

      if (ins->getInsTag() == MInstruction::H_CALL) {
        auto call = static_cast<MHIcall *>(ins.get());
        int float_cnt = 10;
        int int_cnt = 10;
        auto res = vector<MInstruction *>();
        for (int i = 0; i < call->getArgNum(); i++) {
          auto arg = call->getArg(i);
          switch (arg.tp) {
          case MHIcall::Float: {
            auto g =
                func->getMod()->addGlobalFloat(new FloatConstant(arg.arg.f));
            res.push_back(new MIflw(g, Register::getFRegister(float_cnt++)));
            break;
          }
          case MHIcall::Int: {
            res.push_back(
                new MIli(arg.arg.i, Register::getIRegister(int_cnt++)));
            break;
          }
          case MHIcall::Reg: {
            auto offset = allocation->at(arg.arg.reg);
            auto tp = tp_map->at(arg.arg.reg);
            switch (tp) {
            case FLOAT: {
              res.push_back(new MIflw(Register::reg_s0, offset,
                                      Register::getFRegister(float_cnt++)));
              break;
            }
            case INT: {
              res.push_back(new MIlw(Register::reg_s0, offset,
                                     Register::getIRegister(int_cnt++)));
              break;
            }
            case POINTER: {
              res.push_back(new MIld(Register::reg_s0, offset,
                                     Register::getIRegister(int_cnt++)));
              break;
            }
            }
          }
          }
        }
        res.push_back(new MIcall(call->function));
        ins->replaceWith(res);
      }
    }
    for (auto &jmp : bb->getJmps()) {
      if (jmp->getInsTag() == MInstruction::H_RET) {
        auto hret = static_cast<MHIret *>(jmp.get());
        switch (hret->ret_type) {
        case MHIret::Float: {
          auto g =
              func->getMod()->addGlobalFloat(new FloatConstant(hret->fimm));
          bb->pushInstr(new MIflw(g, Register::reg_a0));
          break;
        }
        case MHIret::Int: {
          bb->pushInstr(new MIli(hret->imm, Register::reg_a0));
          break;
        }
        case MHIret::Reg: {
          auto reg = hret->getReg(0);
          if (reg->getTag() == Register::F_REGISTER) {
            bb->pushInstr(new MIfmv_s(reg, Register::reg_fa0));
          } else {
            bb->pushInstr(new MImv(reg, Register::reg_a0));
          }
          break;
        }
        }
        hret->replaceWith({new MIret()});
      }
    }
  }
}

// https://stackoverflow.com/questions/3407012/rounding-up-to-the-nearest-multiple-of-a-number
int roundUp(int numToRound, int multiple) {
  assert(multiple && ((multiple & (multiple - 1)) == 0));
  return (numToRound + multiple - 1) & -multiple;
}

void prelude_conclusion(MFunction *func, map<Register *, uint32_t> *allocation,
                        map<Register *, MTP> *tp_map) {
  // 1. add prelude for each function
  uint32_t stack_zs = roundUp(func->stack_offset, 16);
  auto entry = func->getEntry();
  auto sub_sp = new MIaddi(Register::reg_sp, -stack_zs, Register::reg_sp);
  auto store_ra = new MIsd(Register::reg_ra, stack_zs - 8, Register::reg_sp);
  auto store_fp = new MIsd(Register::reg_s0, stack_zs - 16, Register::reg_sp);
  auto get_fp = new MIaddi(Register::reg_sp, stack_zs, Register::reg_s0);
  auto prelude = vector<MInstruction *>({sub_sp, store_ra, store_fp, get_fp});
  // store arguments
  int float_cnt = 10;
  int int_cnt = 10;
  auto ftp = func->getType();
  for (int i = 0; i < func->getType()->getArgSize(); i++) {
    auto argument = func->getArg(i);
    auto offset = allocation->at(argument);
    auto tp = tp_map->at(argument);
    switch (tp) {
    case FLOAT: {
      prelude.push_back(new MIfsw(Register::getFRegister(float_cnt++), -offset,
                                  Register::reg_s0));
      break;
    }
    case INT: {
      prelude.push_back(new MIsw(Register::getIRegister(int_cnt++), -offset,
                                 Register::reg_s0));
      break;
    }
    case POINTER: {
      prelude.push_back(new MIsd(Register::getIRegister(int_cnt++), -offset,
                                 Register::reg_s0));
      break;
    }
    }
  }

  // no saved registers currently
  entry->pushInstrsAtHead(prelude);

  // 2. add exit block for each function
  auto exit = func->addBasicBlock(func->getName() + "_exit");
  auto load_ra = new MIld(Register::reg_sp, stack_zs - 8, Register::reg_ra);
  auto load_fp = new MIld(Register::reg_sp, stack_zs - 16, Register::reg_s0);
  auto add_sp = new MIaddi(Register::reg_sp, stack_zs, Register::reg_sp);
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

void spill_register_for_func(MFunction *func) {
  // 3.1 allocate register
  uint32_t stack_offset = 16; // with ra and sp

  auto allocation = make_unique<map<Register *, uint32_t>>();
  auto tp_map = make_unique<map<Register *, MTP>>();

  allocate_arguments(func, allocation.get(), tp_map.get(), stack_offset);
  allocate_registers(func, allocation.get(), tp_map.get(), stack_offset);

  // 3.2 rewrite program
  rewrite_program(func, allocation.get(), tp_map.get());

  // 3.3 lower alloca, ret, call
  func->stack_offset = stack_offset;

  lower_hinstrs(func, allocation.get(), tp_map.get());
  prelude_conclusion(func, allocation.get(), tp_map.get());
}

void spill_all_register(MModule *mod) {
  // step1. out-of-ssa
  out_of_ssa(mod);

  // step2: spill registers
  for (auto &func : mod->getFunctions()) {
    spill_register_for_func(func.get());
  }
}

// Linear Scan Register Allocation on SSA Form
static void allocate_register_ssa(MModule *mod) {
  assert(0);
  // step1. Lifetime Analysis
  //// 1.1 Linearize blocks
  //// 1.2 Build Intervals
  // step2. Linear Scan
  // step3. Resolution
}