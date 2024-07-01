#include "select_instruction.hh"
#include <map>

bool is_constant(Value *v) {
  return v->getValueTag() == VT_INTCONST || v->getValueTag() == VT_FLOATCONST;
}

Register *get_vreg(MachineModule *m, vector<MachineInstruction *> &res,
                   Value *v, map<Instruction *, Register *> &instr_map,
                   Function *current_function) {
  if (v->getValueTag() == VT_ARG) {
    auto tp = v->getType();
    auto func_tp = static_cast<FuncType *>(current_function->getType());
    int cnt = 0;
    bool found = false;
    for (int i = 0; i < func_tp->getArgSize(); i++) {
      if (func_tp->getArgument(i) == v) {
        found = true;
        break;
      }
      if (func_tp->getArgument(i)->getType() == tp) {
        cnt++;
      }
    }
    assert(found);
    if (tp == Type::getFloatType()) {
      return getFRegister(10 + cnt);
    } else {
      return getIRegister(10 + cnt);
    }
  }

  if (v->getValueTag() == VT_INTCONST) {
    auto constant_instr = new MachineInstruction(MachineInstruction::ADDIW);
    constant_instr->pushReg(&reg_zero);
    constant_instr->setImm(static_cast<IntegerConstant *>(v)->getValue());
    res.push_back(constant_instr);
    return constant_instr;
  }

  if (v->getValueTag() == VT_FLOATCONST) {
    auto fc = static_cast<FloatConstant *>(v);
    auto fg = m->addGlobalFloat(fc);
    auto load_instr = new MachineInstruction(MachineInstruction::FLW);
    res.push_back(load_instr);
    load_instr->setGlobal(fg);
    return load_instr;
  }

  auto ins = static_cast<Instruction *>(v);
  auto it = instr_map.find(ins);
  if (it != instr_map.end()) {
    return it->second;
  } else {
    return new IRRegister(ins);
  }
}

uint32_t cal_size(const Type *tp) { return 0; }

vector<MachineInstruction *>
select_instruction(MachineModule *m, Instruction &ins,
                   map<Instruction *, Register *> &instr_map,
                   map<BasicBlock *, MachineBasicBlock *> &bb_map,
                   map<Function *, MachineFunction *> &func_map,
                   map<GlobalVariable *, MachineGlobal *> &global_map,
                   Function *current_function) {
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
    opdr = get_vreg(m, res, opd, instr_map, current_function);                 \
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
    opdr = get_vreg(m, res, opd, instr_map, current_function);                 \
  }

#define CREATE_BINARY_INTEGER_INSTR(bin_ins, op_type, opd1_, opd2_)            \
  Register *opdr1;                                                             \
  Register *opdr2;                                                             \
  CREATE_CONSTANT_INTEGER_INSTR(opd1_, opdr1);                                 \
  CREATE_CONSTANT_INTEGER_INSTR(opd2_, opdr2);                                 \
  auto bin_ins = new MachineInstruction(op_type);                              \
  bin_ins->pushReg(opdr1);                                                     \
  bin_ins->pushReg(opdr2);                                                     \
  res.push_back(bin_ins);

#define CREATE_BINARY_FLOAT_INSTR(bin_ins, op_type, opd1_, opd2_)              \
  Register *opdr1;                                                             \
  Register *opdr2;                                                             \
  CREATE_CONSTANT_FLOAT_INSTR(opd1, opdr1);                                    \
  CREATE_CONSTANT_FLOAT_INSTR(opd2, opdr2);                                    \
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
    CREATE_BINARY_FLOAT_INSTR(binary_ins, op_type, opd1, opd2)                 \
    instr_map.insert({&ins, binary_ins});                                      \
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
    BIN_IMM(bin, opi_type,                                                     \
            get_vreg(m, res, opd2, instr_map, current_function), opd1)         \
    bin_ins = bin;                                                             \
  } else if (o2c) {                                                            \
    BIN_IMM(bin, opi_type,                                                     \
            get_vreg(m, res, opd1, instr_map, current_function), opd2)         \
    bin_ins = bin;                                                             \
  } else {                                                                     \
    auto bin = new MachineInstruction(op_type);                                \
    bin->pushReg(get_vreg(m, res, opd1, instr_map, current_function));         \
    bin->pushReg(get_vreg(m, res, opd2, instr_map, current_function));         \
    res.push_back(bin);                                                        \
    bin_ins = bin;                                                             \
  }

#define BINARY_INSTR_WITH_IMM_CASE(op_tag, op_type, opi_type)                  \
  case op_tag: {                                                               \
    CREATE_BINARY_INSTR_WITH_IMM(bin, op_type, opi_type, opd1, opd2)           \
    instr_map.insert({&ins, bin});                                             \
    break;                                                                     \
  }

  case VT_JUMP: {
    JumpInst &jmp = static_cast<JumpInst &>(ins);
    auto mjmp = new MachineInstruction(MachineInstruction::J);
    mjmp->pushJTarget(bb_map.at(static_cast<BasicBlock *>(jmp.getRValue(0))));
    res.push_back(mjmp);
    break;
  }
  case VT_BR: {
    BranchInst &br = static_cast<BranchInst &>(ins);
    auto beq = new MachineInstruction(MachineInstruction::BEQ);
    beq->pushReg(&reg_zero);
    beq->pushReg(
        get_vreg(m, res, br.getRValue(0), instr_map, current_function));
    beq->pushJTarget(bb_map.at(static_cast<BasicBlock *>(br.getRValue(2))));
    res.push_back(beq);
    JumpInst &jmp = static_cast<JumpInst &>(ins);
    auto mjmp = new MachineInstruction(MachineInstruction::J);
    mjmp->pushJTarget(bb_map.at(static_cast<BasicBlock *>(jmp.getRValue(1))));
    res.push_back(mjmp);
    break;
  }
  case VT_CALL: {
    // 1.Place the arguments where the function can access them.
    CallInst &call = static_cast<CallInst &>(ins);
    // todo: use stack when more arguments needed
    int float_cnt = 10;   // f10~f17
    int integer_cnt = 10; // x10~x17
    for (int i = 0; i < call.getRValueSize(); i++) {
      auto arg = call.getRValue(i);
      auto reg = get_vreg(m, res, arg, instr_map, current_function);
      MachineInstruction *move;
      if (reg->is_float()) {
        move = new MachineInstruction(MachineInstruction::FADD_S);
        move->pushReg(getFRegister(float_cnt++));
      } else {
        move = new MachineInstruction(MachineInstruction::ADDW);
        move->pushReg(getIRegister(integer_cnt++));
      }
      move->pushReg(reg);
      res.push_back(move);
    }
    // 2.Jump to the function (using presudo instruction call)..
    // todo: maybe we should abstract all these things to Value like llvm IR
    // todo: maybe an extended version of llvm IR is needed here
    auto mcall = new MachineInstruction(MachineInstruction::CALL);
    // todo: IR bug -> call Instr 不知道自己 call 谁
    mcall->setFunction(func_map.at(call.getFunction()));
    res.push_back(mcall);
    if (static_cast<FuncType>(call.getType()).getRetType() ==
        Type::getFloatType()) {
      instr_map.insert({&ins, getFRegister(10)});
    } else {
      instr_map.insert({&ins, getIRegister(10)});
    }
    break;
  }
  case VT_RET: {
    auto ret = new MachineInstruction(MachineInstruction::RET);
    res.push_back(ret);
    break;
  }
  case VT_FPTOSI: {
    FptosiInst &i = static_cast<FptosiInst &>(ins);
    auto fcvtw_s = new MachineInstruction(MachineInstruction::FCVTW_S);
    Register *opdr;
    CREATE_CONSTANT_FLOAT_INSTR(ins.getRValue(0), opdr);
    fcvtw_s->pushReg(opdr);
    res.push_back(fcvtw_s);
    instr_map.insert({&ins, fcvtw_s});
    break;
  }
  case VT_SITOFP: {
    SitofpInst &i = static_cast<SitofpInst &>(ins);
    auto fcvts_w = new MachineInstruction(MachineInstruction::FCVTS_W);
    Register *opdr;
    CREATE_CONSTANT_INTEGER_INSTR(ins.getRValue(0), opdr);
    fcvts_w->pushReg(opdr);
    res.push_back(fcvts_w);
    instr_map.insert({&ins, fcvts_w});
    break;
  }
  case VT_ALLOCA: {
    AllocaInst &alloca = static_cast<AllocaInst &>(ins);
    auto tp = alloca.getType();
    auto size = cal_size(tp);
    auto push = new MachineInstruction(MachineInstruction::ADDIW);
    push->pushReg(&reg_sp);
    push->setImm(Immediate(size));
    push->setTarget(&reg_sp);
    res.push_back(push);
    instr_map.insert({&ins, &reg_sp});
    break;
  }
  case VT_LOAD: {
    // depend on loaded type and
    // 1. load pointer(get pointer through alloca, no heap alloaction)
    // 2. load global variale
    LoadInst &load = static_cast<LoadInst &>(ins);
    auto tp = load.getType();
    auto addr = load.getRValue(0);
    MachineInstruction *mload;
    if (tp == Type::getFloatType()) {
      mload = new MachineInstruction(MachineInstruction::FLW);
    } else {
      mload = new MachineInstruction(MachineInstruction::LW);
    }
    if (addr->getValueTag() == VT_GLOBALVAR) {
      mload->setGlobal(global_map.at(static_cast<GlobalVariable *>(addr)));
    } else {
      mload->pushReg(get_vreg(m, res, addr, instr_map, current_function));
    }
    res.push_back(mload);
    instr_map.insert({&ins, mload});
    break;
  }
  case VT_STORE: {
    // store float or integer
    // 1. to global
    // 2. to pointer
    StoreInst &store = static_cast<StoreInst &>(ins);
    auto value = store.getRValue(0);
    auto addr = store.getRValue(1);
    MachineInstruction *mstore;
    if (value->getType() == Type::getFloatType()) {
      mstore = new MachineInstruction(MachineInstruction::FSW);
    } else {
      mstore = new MachineInstruction(MachineInstruction::SW);
    }
    if (addr->getValueTag() == VT_GLOBALVAR) {
      mstore->setGlobal(global_map.at(static_cast<GlobalVariable *>(addr)));
    } else {
      // todo: get_vreg should handle the immidiet case!!!!
      mstore->pushReg(get_vreg(m, res, addr, instr_map, current_function));
    }
    mstore->pushReg(get_vreg(m, res, value, instr_map, current_function));
    res.push_back(mstore);
    break;
  }
  case VT_GEP: {
    // calculate address
    GetElemPtrInst &gep = static_cast<GetElemPtrInst &>(ins);

    Register *base =
        get_vreg(m, res, gep.getRValue(0), instr_map, current_function);

    const Type *current_type = gep.getPtrType();
    MachineInstruction *dest;
    for (unsigned i = 1; i < gep.getRValueSize(); i++) {
      Register *index =
          get_vreg(m, res, gep.getRValue(i), instr_map, current_function);

      MachineInstruction *elesz =
          new MachineInstruction(MachineInstruction::ADDIW);
      elesz->pushReg(&reg_zero);
      elesz->setImm(Immediate(cal_size(current_type)));
      if (i != gep.getRValueSize() - 1) {
        current_type =
            static_cast<const ArrayType *>(current_type)->getElemType();
      }
      res.push_back(elesz);

      MachineInstruction *offset =
          new MachineInstruction(MachineInstruction::MULW);
      offset->pushReg(index);
      offset->pushReg(elesz);
      res.push_back(offset);

      MachineInstruction *add =
          new MachineInstruction(MachineInstruction::ADDIW);
      add->pushReg(base);
      add->pushReg(offset);
      res.push_back(add);
      dest = add;
    }
    instr_map.insert({&ins, dest});
    break;
  }
  case VT_PHI: {
    PhiInst &phi = static_cast<PhiInst &>(ins);
    // 遍历 Phi 指令的所有操作数
    MachineInstruction *mphi = new MachineInstruction(MachineInstruction::PHI);
    for (int i = 0; i < phi.getRValueSize(); i += 2) {
      // 获取操作数和对应的前驱基本块
      Value *operand = phi.getRValue(i);
      BasicBlock *pred_bb = static_cast<BasicBlock *>(phi.getRValue(i + 1));
      mphi->pushReg(get_vreg(m, res, operand, instr_map, current_function));
      mphi->pushJTarget(bb_map.at(pred_bb));
    }
    instr_map.insert({&ins, mphi});
    break;
  }
  case VT_ZEXT: {
    ZextInst &zt = static_cast<ZextInst &>(ins);
    Register *oprand =
        get_vreg(m, res, zt.getRValue(0), instr_map, current_function);
    instr_map.insert({&ins, oprand});
    break;
  }
  case VT_ICMP: {
    IcmpInst &icmp = static_cast<IcmpInst &>(ins);
    auto opd1 = icmp.getRValue(0);
    auto opd2 = icmp.getRValue(1);
    switch (icmp.getOpTag()) {
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
    default:
      assert(0);
    }
    break;
  }
  case VT_FCMP: {
    FcmpInst &fcmp = static_cast<FcmpInst &>(ins);
    auto opd1 = fcmp.getRValue(0);
    auto opd2 = fcmp.getRValue(1);
    switch (fcmp.getOpTag()) {
      BINARY_FLOAT_INSTR_CASE(OEQ, MachineInstruction::FEQ_S)
    case ONE: {
      CREATE_BINARY_FLOAT_INSTR(feq_ins, MachineInstruction::FEQ_S, opd2, opd1)
      MachineInstruction *neq_ins =
          new MachineInstruction(MachineInstruction::XORI);
      neq_ins->setImm(Immediate(1));
      neq_ins->pushReg(feq_ins);
      res.push_back(neq_ins);
      instr_map.insert({&ins, neq_ins});
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
    default:
      assert(0);
    }
    break;
  }
  case VT_BOP: {
    BinaryOpInst &bins = static_cast<BinaryOpInst &>(ins);
    auto opd1 = bins.getRValue(0);
    auto opd2 = bins.getRValue(1);
    switch (bins.getOpTag()) {

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

    default:
      assert(0);
    }
    break;
  }
  default:
    assert(0);
  }

  return res;
}

void select_instruction(MachineModule *res, ANTPIE::Module *ir) {
  auto instr_map = new map<Instruction *, Register *>();
  auto bb_map = new map<BasicBlock *, MachineBasicBlock *>();
  auto func_map = new map<Function *, MachineFunction *>();
  auto global_map = new map<GlobalVariable *, MachineGlobal *>();

  for (const auto &global : ir->getGlobalVariables()) {
    MachineGlobal *g = res->addGlobalVariable(&*global);
    global_map->insert({&*global, g});
  }

  // create function relations
  for (const auto &func : ir->getFunctions()) {
    MachineFunction *mfunc = res->addFunction(
        static_cast<FuncType *>(func->getType()), func->getName());
    func_map->insert({&*func, mfunc});
  }

  for (const auto &func : ir->getFunctions()) {
    MachineFunction *mfunc = func_map->at(&*func);

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
        auto minstrs = select_instruction(res, *i, *instr_map, *bb_map,
                                         *func_map, *global_map, &*func);
        for (auto minstr : minstrs) {
          std::cout << "  ins:";
          minstr->printASM(std::cout);
          std::cout << std::endl;
        }
        mbb->pushInstrs(minstrs);
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
