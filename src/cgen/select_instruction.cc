#include "select_instruction.hh"
#include <map>

bool is_constant(Value *v) {
  return v->getValueTag() == VT_INTCONST || v->getValueTag() == VT_FLOATCONST;
}

Register *get_vreg(Value *v,
                   map<Instruction *, MachineInstruction *> &instr_map) {
  auto ins = static_cast<Instruction *>(v);
  if (instr_map.find(ins) != instr_map.end()) {
    return instr_map.at(ins);
  } else {
    return new IRRegister(ins);
  }
}

vector<MachineInstruction *>
select_instruction(MachineModule *m, Instruction &ins,
                   map<Instruction *, MachineInstruction *> &instr_map,
                   map<BasicBlock *, MachineBasicBlock *> &bb_map) {
  auto res = vector<MachineInstruction *>();
  switch (ins.getValueTag()) {

#define CREATE_CONSTANT_INTEGER_INSTR(opd, opdr)                               \
  if (is_constant(opd)) {                                                      \
    auto constant_instr = new MachineInstruction(MachineInstruction::ADDIW);   \
    constant_instr->pushReg(&reg_zero);                                        \
    constant_instr->setImm(static_cast<IntegerConstant *>(opd)->getValue());   \
    res.push_back(constant_instr);                                             \
    opdr = constant_instr;                                                     \
  } else {                                                                     \
    opdr = get_vreg(opd, instr_map);                                           \
  }

#define CREATE_CONSTANT_FLOAT_INSTR(opd, opdr)                                 \
  if (is_constant(opd)) {                                                      \
    auto fc = static_cast<FloatConstant *>(opd);                               \
    auto fg = m->addGlobalFloat(fc);                                           \
    auto load_instr = new MachineInstruction(MachineInstruction::FLW);         \
    res.push_back(load_instr);                                                 \
    load_instr->setGlobal(fg);                                                 \
    opdr = load_instr;                                                         \
  } else {                                                                     \
    opdr = get_vreg(opd, instr_map);                                           \
  }

  case VT_JUMP: {
    JumpInst &jmp = static_cast<JumpInst &>(ins);
    auto mjmp = new MachineInstruction(MachineInstruction::JAL);
    mjmp->pushTarget(bb_map.at(static_cast<BasicBlock *>(jmp.getRValue(0))));
    res.push_back(mjmp);
  }
  case VT_BR: {
    BranchInst &br = static_cast<BranchInst &>(ins);
    auto beq = new MachineInstruction(MachineInstruction::BEQ);
    beq->pushReg(&reg_zero);
    beq->pushReg(get_vreg(br.getRValue(0), instr_map));
    beq->pushTarget(bb_map.at(static_cast<BasicBlock *>(br.getRValue(2))));
    res.push_back(beq);
    JumpInst &jmp = static_cast<JumpInst &>(ins);
    auto mjmp = new MachineInstruction(MachineInstruction::JAL);
    mjmp->pushTarget(bb_map.at(static_cast<BasicBlock *>(jmp.getRValue(1))));
    res.push_back(mjmp);
    break;
  }
  case VT_RET:
    // recover sp, fp...
    // save returned value to specific register
    // jalr zero, ra, 0
    break;
  case VT_CALL: {
    // 1.pass arguments
    // 2. save sp, fp...
    // 3.jal ra, function
    // 4.move return value to virtual register
    break;
  }
  case VT_FPTOSI: {
    FptosiInst &i = static_cast<FptosiInst &>(ins);
    auto fcvtw_s = new MachineInstruction(MachineInstruction::FCVTW_S);
    Register *opdr;
    CREATE_CONSTANT_FLOAT_INSTR(ins.getRValue(0), opdr);
    fcvtw_s->pushReg(opdr);
    res.push_back(fcvtw_s);
  }
  case VT_SITOFP: {
    SitofpInst &i = static_cast<SitofpInst &>(ins);
    auto fcvts_w = new MachineInstruction(MachineInstruction::FCVTS_W);
    Register *opdr;
    CREATE_CONSTANT_INTEGER_INSTR(ins.getRValue(0), opdr);
    fcvts_w->pushReg(opdr);
    res.push_back(fcvts_w);
  }
  case VT_ALLOCA: {
    // sp++ (depend on alloca type)
    // return address
    break;
  }
  case VT_LOAD: {
    // depend on loaded type and
    // 1. load pointer(get pointer through alloca, no heap alloaction)
    // 2. load global variale
    break;
  }
  case VT_STORE: {
    // depend on stored type and
    break;
  }
  case VT_GEP: {
    // calculate address
    break;
  }
  case VT_PHI: {
    // PHI
    break;
  }
  case VT_ZEXT: {
    break;
  }
  case VT_ICMP: {
    break;
  }
  case VT_FCMP: {
    break;
  }
  case VT_BOP: {
    BinaryOpInst &bins = static_cast<BinaryOpInst &>(ins);
    auto opd1 = bins.getRValue(0);
    auto opd2 = bins.getRValue(1);
    switch (bins.getOpTag()) {

#define CREATE_BINARY_INTEGER_INSTR(bin_ins, op_type, opd1_, opd2_)            \
  Register *opdr1;                                                             \
  Register *opdr2;                                                             \
  CREATE_CONSTANT_INTEGER_INSTR(opd1_, opdr1);                                 \
  CREATE_CONSTANT_INTEGER_INSTR(opd2_, opdr2);                                 \
  auto bin_ins = new MachineInstruction(op_type);                              \
  bin_ins->pushReg(opdr1);                                                     \
  bin_ins->pushReg(opdr2);                                                     \
  res.push_back(bin_ins);

#define BINARY_INTEGER_INSTR_CASE(op_tag, op_type)                             \
  case op_tag: {                                                               \
    CREATE_BINARY_INTEGER_INSTR(binary_ins, op_type, opd1, opd2)               \
    instr_map.insert({&ins, binary_ins});                                      \
    break;                                                                     \
  }

#define BINARY_FLOAT_INSTR_CASE(op_tag, op_type)                               \
  case op_tag: {                                                               \
    Register *opdr1;                                                           \
    Register *opdr2;                                                           \
    CREATE_CONSTANT_FLOAT_INSTR(opd1, opdr1);                                  \
    CREATE_CONSTANT_FLOAT_INSTR(opd2, opdr2);                                  \
    auto binary_ins = new MachineInstruction(op_type);                         \
    binary_ins->pushReg(opdr1);                                                \
    binary_ins->pushReg(opdr2);                                                \
    instr_map.insert({&ins, binary_ins});                                      \
    res.push_back(binary_ins);                                                 \
    break;                                                                     \
  }

#define BIN_IMM(bin_ins, opi_type, reg, imm)                                   \
  auto bin_ins = new MachineInstruction(opi_type);                             \
  bin_ins->pushReg(reg);                                                       \
  bin_ins->setImm(static_cast<IntegerConstant *>(imm)->getValue());            \
  res.push_back(bin_ins);

#define CREATE_BINARY_INSTR_WITH_IMM(bin_ins, op_type, opi_type, opd1_, opd2_) \
  MachineInstruction *bin_ins;                                                 \
  auto o1c = is_constant(opd1_);                                               \
  auto o2c = is_constant(opd2_);                                               \
  if (o1c && o2c) {                                                            \
    BIN_IMM(bin1, opi_type, &reg_zero, opd1_)                                  \
    BIN_IMM(bin2, opi_type, &reg_zero, opd2_)                                  \
    bin_ins = bin2;                                                            \
  } else if (o1c) {                                                            \
    BIN_IMM(bin, opi_type, get_vreg(opd2, instr_map), opd1)                    \
    bin_ins = bin;                                                             \
  } else if (o2c) {                                                            \
    BIN_IMM(bin, opi_type, get_vreg(opd1, instr_map), opd2)                    \
    bin_ins = bin;                                                             \
  } else {                                                                     \
    auto bin = new MachineInstruction(op_type);                                \
    bin->pushReg(get_vreg(opd1, instr_map));                                   \
    bin->pushReg(get_vreg(opd2, instr_map));                                   \
    res.push_back(bin);                                                        \
    bin_ins = bin;                                                             \
  }

#define BINARY_INSTR_WITH_IMM_CASE(op_tag, op_type, opi_type)                  \
  case op_tag: {                                                               \
    CREATE_BINARY_INSTR_WITH_IMM(bin, op_type, opi_type, opd1, opd2)           \
    instr_map.insert({&ins, bin});                                             \
    break;                                                                     \
  }

      BINARY_INSTR_WITH_IMM_CASE(ADD, MachineInstruction::ADDW,
                                 MachineInstruction::ADDIW)
      BINARY_INSTR_WITH_IMM_CASE(AND, MachineInstruction::AND,
                                 MachineInstruction::ANDI)
      BINARY_INSTR_WITH_IMM_CASE(OR, MachineInstruction::OR,
                                 MachineInstruction::ORI)
      BINARY_INSTR_WITH_IMM_CASE(XOR, MachineInstruction::XOR,
                                 MachineInstruction::XORI)

      BINARY_INTEGER_INSTR_CASE(SUB, MachineInstruction::SUBW)
      BINARY_INTEGER_INSTR_CASE(MUL, MachineInstruction::MULW)
      BINARY_INTEGER_INSTR_CASE(SDIV, MachineInstruction::DIVW)
      BINARY_INTEGER_INSTR_CASE(SREM, MachineInstruction::REMW)

      BINARY_FLOAT_INSTR_CASE(FADD, MachineInstruction::FADD_S)
      BINARY_FLOAT_INSTR_CASE(FSUB, MachineInstruction::FSUB_S)
      BINARY_FLOAT_INSTR_CASE(FMUL, MachineInstruction::FMUL_S)
      BINARY_FLOAT_INSTR_CASE(FDIV, MachineInstruction::FDIV_S)
    case FREM: {
      // todo
      // what si frem?
      assert(0);
    }

    case EQ: {
      CREATE_BINARY_INTEGER_INSTR(sub_ins, MachineInstruction::SUBW, opd1, opd2)
      MachineInstruction *eq_zero =
          new MachineInstruction(MachineInstruction::SLTIU);
      eq_zero->setImm(Immediate(1));
      eq_zero->pushReg(sub_ins);
      res.push_back(eq_zero);
      instr_map.insert({&ins, eq_zero});
      break;
    }
    case NE: {
      CREATE_BINARY_INTEGER_INSTR(sub_ins, MachineInstruction::SUBW, opd1, opd2)
      MachineInstruction *sltu_ins =
          new MachineInstruction(MachineInstruction::SLTU);
      sltu_ins->pushReg(&reg_zero);
      sltu_ins->pushReg(sub_ins);
      res.push_back(sltu_ins);
      instr_map.insert({&ins, sltu_ins});
      break;
    }
    case SLE: {
      CREATE_BINARY_INTEGER_INSTR(slt_ins, MachineInstruction::SLT, opd2, opd1)
      MachineInstruction *sge_ins =
          new MachineInstruction(MachineInstruction::XORI);
      sge_ins->setImm(Immediate(1)); // 我们将使用XORI将SLT的结果取反
      sge_ins->pushReg(slt_ins);     // SLT的结果作为第一个操作数
      res.push_back(sge_ins);
      instr_map.insert({&ins, sge_ins});
      break;
    }
      BINARY_INTEGER_INSTR_CASE(SLT, MachineInstruction::SLT)
    case SGE: {
      CREATE_BINARY_INTEGER_INSTR(slt_ins, MachineInstruction::SLT, opd1, opd2)
      MachineInstruction *sge_ins =
          new MachineInstruction(MachineInstruction::XORI);
      sge_ins->setImm(Immediate(1)); // 我们将使用XORI将SLT的结果取反
      sge_ins->pushReg(slt_ins);     // SLT的结果作为第一个操作数
      res.push_back(sge_ins);
      instr_map.insert({&ins, sge_ins});
      break;
    }
    case SGT: {
      // 对于SGT（Signed Greater Than），我们可以使用SLT并交换操作数
      CREATE_BINARY_INTEGER_INSTR(slt_ins, MachineInstruction::SLT, opd2, opd1)
      instr_map.insert({&ins, slt_ins});
      break;
    }
      BINARY_FLOAT_INSTR_CASE(OEQ, MachineInstruction::FEQ_S)
    case ONE: {
      break;
    }
      BINARY_FLOAT_INSTR_CASE(OLT, MachineInstruction::FLT_S)
      BINARY_FLOAT_INSTR_CASE(OLE, MachineInstruction::FLE_S)
    case OGT: {
      CREATE_BINARY_INTEGER_INSTR(olt_ins, MachineInstruction::SLT, opd2, opd1)
      instr_map.insert({&ins, olt_ins});
      break;
    }
    case OGE: {
      CREATE_BINARY_INTEGER_INSTR(ole_ins, MachineInstruction::SLT, opd2, opd1)
      instr_map.insert({&ins, ole_ins});
      break;
    }
    default: {
      assert(0);
    }
    }
    break;
  }
  default:
    assert(0);
  }

  return res;
}

void select_instruction(MachineModule *res, ANTPIE::Module *ir) {
  auto instr_map = new map<Instruction *, MachineInstruction *>();
  auto bb_map = new map<BasicBlock *, MachineBasicBlock *>();

  for (const auto &func : ir->getFunctions()) {
    MachineFunction *mfunc = res->addFunction(
        static_cast<FuncType *>(func->getType()), func->getName());

    // create basicblock relations
    for (const auto &bb : func->getBasicBlocks()) {
      MachineBasicBlock *mbb = res->addBasicBlock(mfunc, bb->getName());
      bb_map->insert({&*bb, mbb});
    }
    for (const auto &bb : func->getBasicBlocks()) {
      MachineBasicBlock *mbb = bb_map->at(&*bb);
      for (const auto &i : bb->getInstructions()) {
        std::cout << "select instruction for  ";
        i->printIR(std::cout);
        std::cout << std::endl;
        mbb->pushInstrs(select_instruction(res, *i, *instr_map, *bb_map));
      }
    }
  }

  // reslove IRRegisters to VRegisters
  for (auto &func : res->getFunctions()) {
    for (auto &bb : func->getBasicBlocks()) {
      for (auto &ins : bb->getInstructions()) {
        ins->replaceIRRegister(*instr_map);
      }
    }
  }
}
