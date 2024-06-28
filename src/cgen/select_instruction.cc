#include "select_instruction.hh"
#include <map>

Register *select_oprand(Value *opd,
                        map<Instruction *, MachineInstruction *> &instr_map) {
  if (opd->getValueTag() == VT_VALUE) {
    auto c = dynamic_cast<Constant *>(opd);
    if (c->isIntConst()) {
      return &reg_a0;
    } else if (c->isFloatConst()) {
      return &reg_a0;
    } else {
      assert(0);
    }
  }
}

// Register *select_operand(Value *opd,
//                          map<Instruction *, MachineInstruction *> &instr_map)
//                          {
//   switch (opd->getValueTag()) {
//   case VT_VALUE: // const
//     auto c = dynamic_cast<Constant *>(opd);
//     if (c->isIntConst()) {
//       // 处理整型常量
//       return /* 返回相应的 Register * */;
//     } else if (c->isFloatConst()) {
//       // 处理浮点常量
//       return /* 返回相应的 Register * */;
//     } else {
//       assert(0);
//     }
//     return /* 返回合适的 Register * */;
//   default: // Instruction Virtual Register
//     assert(0);
//   }
// }

bool is_constant(Value *v) { return v->getValueTag() == VT_VALUE; }

vector<MachineInstruction *>
select_instruction(MachineModule *m, Instruction &ins,
                   map<Instruction *, MachineInstruction *> &instr_map,
                   map<BasicBlock *, MachineBasicBlock *> &bb_map) {
  auto res = new vector<MachineInstruction *>();
  switch (ins.getValueTag()) {
  case VT_JUMP:
    /* code */
    break;
  case VT_RET:
    /* code */
    break;
  case VT_BR:
    /* code */
    break;
  case VT_CALL:
    /* code */
    break;
  case VT_FPTOSI:
    /* code */
    break;
  case VT_SITOFP:
    /* code */
    break;
  case VT_ALLOCA:
    /* code */
    break;
  case VT_LOAD:
    // load pointer(get pointer through alloca, no heap alloaction)
    // load global variale
    break;
  case VT_STORE:
    /* code */
    break;
  case VT_GEP:
    /* code */
    break;
  case VT_PHI:
    /* code */
    break;
  case VT_ZEXT:
    /* code */
    break;
  case VT_ICMP:
    /* code */
    break;
  case VT_FCMP:
    /* code */
    break;
  case VT_BOP: {
    BinaryOpInst &bins = static_cast<BinaryOpInst &>(ins);
    auto opd1 = bins.getRValue(0);
    auto o1c = is_constant(bins.getRValue(0)); // opd1 is constant
    auto opd2 = bins.getRValue(1);
    auto o2c = is_constant(bins.getRValue(1)); // opd2 is constant
    switch (bins.getOpTag()) {
    case ADD: {
      if (o1c && o2c) {
        auto add1 = new MachineInstruction(MachineInstruction::ADDIW);
        add1->pushReg(&reg_zero);
        add1->setImm(static_cast<IntegerConstant *>(opd1)->getValue());
        auto add2 = new MachineInstruction(MachineInstruction::ADDIW);
        add2->pushReg(add1);
        add2->setImm(static_cast<IntegerConstant *>(opd2)->getValue());
        instr_map.insert({&ins, add2});
        res->push_back(add1);
        res->push_back(add2);
      } else if (o1c) {
        auto add = new MachineInstruction(MachineInstruction::ADDIW);
        add->pushReg(instr_map.at(static_cast<Instruction *>(opd2)));
        add->setImm(static_cast<IntegerConstant *>(opd1)->getValue());
        instr_map.insert({&ins, add});
        res->push_back(add);
      } else if (o2c) {
        auto add = new MachineInstruction(MachineInstruction::ADDIW);
        add->pushReg(instr_map.at(static_cast<Instruction *>(opd1)));
        add->setImm(static_cast<IntegerConstant *>(opd2)->getValue());
        instr_map.insert({&ins, add});
        res->push_back(add);
      } else {
        auto add = new MachineInstruction(MachineInstruction::ADDW);
        add->pushReg(instr_map.at(static_cast<Instruction *>(opd1)));
        add->pushReg(instr_map.at(static_cast<Instruction *>(opd2)));
        instr_map.insert({&ins, add});
        res->push_back(add);
      }
      break;
    }
    case FADD: {
      Register *opdr1;
      Register *opdr2;
      if (o1c) {
        auto fc = static_cast<FloatConstant *>(opd1);
        auto fg = m->addGlobalFloat(fc);
        auto opdr1 = new MachineInstruction(MachineInstruction::FLW);
        res->push_back(opdr1);
        opdr1->setGlobal(fg);
      } else {
        opdr1 = instr_map.at(static_cast<Instruction *>(opd1));
      }

      if (o2c) {
        auto fc = static_cast<FloatConstant *>(opd2);
        auto fg = m->addGlobalFloat(fc);
        auto opdr2 = new MachineInstruction(MachineInstruction::FLW);
        res->push_back(opdr2);
        opdr2->setGlobal(fg);
      } else {
        opdr2 = instr_map.at(static_cast<Instruction *>(opd2));
      }

      auto add = new MachineInstruction(MachineInstruction::FADD_S);
      add->pushReg(opdr1);
      add->pushReg(opdr2);
      instr_map.insert({&ins, add});
      res->push_back(add);
      break;
    }
    case SUB: {
      Register *opdr1;
      Register *opdr2;
      if (o1c) {
        auto opdr1 = new MachineInstruction(MachineInstruction::ADDIW);
        opdr1->pushReg(&reg_zero);
        opdr1->setImm(static_cast<IntegerConstant *>(opd1)->getValue());
        res->push_back(opdr1);
      } else {
        opdr1 = instr_map.at(static_cast<Instruction *>(opd1));
      }

      if (o2c) {
        auto opdr2 = new MachineInstruction(MachineInstruction::ADDIW);
        opdr2->pushReg(&reg_zero);
        opdr2->setImm(static_cast<IntegerConstant *>(opd2)->getValue());
        res->push_back(opdr2);
      } else {
        opdr2 = instr_map.at(static_cast<Instruction *>(opd2));
      }
      auto sub = new MachineInstruction(MachineInstruction::SUBW);
      sub->pushReg(opdr1);
      sub->pushReg(opdr2);
      instr_map.insert({&ins, sub});
      res->push_back(sub);
      break;
    }
    case FSUB: {
      Register *opdr1;
      Register *opdr2;
      if (o1c) {
        auto fc = static_cast<FloatConstant *>(opd1);
        auto fg = m->addGlobalFloat(fc);
        auto opdr1 = new MachineInstruction(MachineInstruction::FLW);
        res->push_back(opdr1);
        opdr1->setGlobal(fg);
      } else {
        opdr1 = instr_map.at(static_cast<Instruction *>(opd1));
      }

      if (o2c) {
        auto fc = static_cast<FloatConstant *>(opd2);
        auto fg = m->addGlobalFloat(fc);
        auto opdr2 = new MachineInstruction(MachineInstruction::FLW);
        res->push_back(opdr2);
        opdr2->setGlobal(fg);
      } else {
        opdr2 = instr_map.at(static_cast<Instruction *>(opd2));
      }

      auto sub = new MachineInstruction(MachineInstruction::FSUB_S);
      sub->pushReg(opdr1);
      sub->pushReg(opdr2);
      instr_map.insert({&ins, sub});
      res->push_back(sub);
      break;
    }
    case MUL:
      // TODO: use macro to avoid reduction
      // TODO: Is there a better way of instruction selection?
      { break; }
    case FMUL: {
      break;
    }
    case SDIV: {
      break;
    }
    case FDIV: {
      break;
    }
    case SREM: {
      break;
    }
    case FREM: {
      break;
    }
    case AND: {
      break;
    }
    case OR: {
      break;
    }
    case XOR: {
      break;
    }
    case EQ: {
      break;
    }
    case NE: {
      break;
    }
    case SLE: {
      break;
    }
    case SLT: {
      break;
    }
    case SGE: {
      break;
    }
    case SGT: {
      break;
    }
    case OEQ: {
      break;
    }
    case ONE: {
      break;
    }
    case OGT: {
      break;
    }
    case OGE: {
      break;
    }
    case OLE: {
      break;
    }
    case OLT: {
      break;
    }
    default: {
      break;
    }
    }
    break;
  }
  default:
    assert(0);
  }
}

void select_instruction(MachineModule *res, ANTPIE::Module *ir) {
  auto instr_map = new map<Instruction *, MachineInstruction *>();
  auto bb_map = new map<BasicBlock *, MachineBasicBlock *>();

  for (const auto &func : ir->getFunctions()) {
    MachineFunction *mfunc = res->addFunction(
        static_cast<FuncType *>(func->getType()), func->getName());
    for (const auto &bb : func->getBasicBlocks()) {
      MachineBasicBlock *mbb = res->addBasicBlock(mfunc, bb->getName());
      for (const auto &i : bb->getInstructions()) {
        mbb->pushInstrs(select_instruction(res, *i, *instr_map, *bb_map));
      }
    }
  }
}
