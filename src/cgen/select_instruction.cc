#include "select_instruction.hh"
#include <map>

bool is_constant(Value *v) {
  return v->getValueTag() == VT_INTCONST || v->getValueTag() == VT_FLOATCONST;
}

// capture (res)
#define ADD_INSTR(INSTR, CONS, ...)                                            \
  auto INSTR = new CONS(__VA_ARGS__);                                          \
  res.push_back(INSTR)

// capture (m ,res, instr_map, current_function)
#define GET_VREG(V) get_vreg(m, res, V, instr_map, current_function)

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
    ADD_INSTR(i, MIli, static_cast<IntegerConstant *>(v)->getValue());
    return i;
  }

  if (v->getValueTag() == VT_FLOATCONST) {
    auto fc = static_cast<FloatConstant *>(v);
    auto fg = m->addGlobalFloat(fc);
    ADD_INSTR(i, MIflw, fg);
    return i;
  }

  auto ins = static_cast<Instruction *>(v);
  auto it = instr_map.find(ins);
  if (it != instr_map.end()) {
    return it->second;
  } else {
    return new IRRegister(ins);
  }
}

vector<MachineInstruction *>
select_instruction(MachineModule *m, Instruction &ins,
                   map<Instruction *, Register *> &instr_map,
                   map<BasicBlock *, MachineBasicBlock *> &bb_map,
                   map<Function *, MachineFunction *> &func_map,
                   map<GlobalVariable *, MachineGlobal *> &global_map,
                   Function *current_function) {
  auto res = vector<MachineInstruction *>();
  switch (ins.getValueTag()) {

#define CREATE_BINARY_INSTR_WITH_IMM(BIN_INS, OP_CLASS, OPI_CLASS, OPD1, OPD2, \
                                     CONST_TP)                                 \
  MachineInstruction *BIN_INS;                                                 \
  auto o1c = is_constant(OPD1);                                                \
  auto o2c = is_constant(OPD2);                                                \
  if (o1c && o2c) {                                                            \
    ADD_INSTR(bin, OP_CLASS, GET_VREG(OPD1), GET_VREG(OPD2));                  \
    BIN_INS = bin;                                                             \
  } else if (o1c) {                                                            \
    auto imm = static_cast<CONST_TP *>(OPD1)->getValue();                      \
    ADD_INSTR(bin, OPI_CLASS, GET_VREG(OPD2), imm);                            \
    BIN_INS = bin;                                                             \
  } else if (o2c) {                                                            \
    auto imm = static_cast<CONST_TP *>(OPD2)->getValue();                      \
    ADD_INSTR(bin, OPI_CLASS, GET_VREG(OPD1), imm);                            \
    BIN_INS = bin;                                                             \
  } else {                                                                     \
    ADD_INSTR(bin, OP_CLASS, GET_VREG(OPD1), GET_VREG(OPD2));                  \
    BIN_INS = bin;                                                             \
  }

#define BINARY_INSTR_WITH_IMM_CASE(op_tag, op_class, opi_class, CONST_TP)      \
  case op_tag: {                                                               \
    CREATE_BINARY_INSTR_WITH_IMM(bin, op_class, opi_class, opd1, opd2,         \
                                 CONST_TP)                                     \
    auto name = ins.getName();                                                 \
    bin->setName(name);                                                        \
    instr_map.insert({&ins, bin});                                             \
    break;                                                                     \
  }

#define BINARY_OP_CASE(OP, INSTR)                                              \
  case OP: {                                                                   \
    ADD_INSTR(i, INSTR, GET_VREG(opd1), GET_VREG(opd2), ins.getName());        \
    instr_map.insert({&ins, i});                                               \
    break;                                                                     \
  }

#define BINARY_OP_REVERSE(OP, INSTR)                                           \
  case OP: {                                                                   \
    ADD_INSTR(i, INSTR, GET_VREG(opd1), GET_VREG(opd2), ins.getName());        \
    instr_map.insert({&ins, i});                                               \
    break;                                                                     \
  }

  case VT_JUMP: {
    JumpInst &jmp = static_cast<JumpInst &>(ins);
    auto mbb = bb_map.at(static_cast<BasicBlock *>(jmp.getRValue(0)));
    ADD_INSTR(mjmp, MIj, mbb);
    break;
  }
  case VT_BR: {
    BranchInst &br = static_cast<BranchInst &>(ins);
    auto cond = GET_VREG(br.getRValue(0));
    auto if_true = bb_map.at(static_cast<BasicBlock *>(br.getRValue(2)));
    auto if_false = bb_map.at(static_cast<BasicBlock *>(br.getRValue(2)));
    ADD_INSTR(beq, MIbeq, &reg_zero, cond, if_false);
    ADD_INSTR(jmp, MIj, if_true);
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
      if (reg->is_float()) {
        ADD_INSTR(_, MIfmv_s, reg, getFRegister(float_cnt++));
      } else {
        ADD_INSTR(_, MImv, reg, getFRegister(integer_cnt++));
      }
    }
    // 2.Jump to the function (using presudo instruction call)..
    auto func = func_map.at(call.getFunction());
    ADD_INSTR(mcall, MIcall, func);

    if (static_cast<FuncType>(call.getType()).getRetType() ==
        Type::getFloatType()) {
      ADD_INSTR(move, MIfmv_s, getFRegister(10), ins.getName());
      instr_map.insert({&ins, move});
    } else {
      ADD_INSTR(move, MImv, getIRegister(10), ins.getName());
      instr_map.insert({&ins, move});
    }
    break;
  }
  case VT_RET: {
    ReturnInst &ret = static_cast<ReturnInst &>(ins);
    auto ret_val = ret.getRValue(0);
    auto ret_val_reg = GET_VREG(ret_val);
    if (ret_val->getType() == Type::getFloatType()) {
      ADD_INSTR(move, MIfmv_s, ret_val_reg, getFRegister(10));
    } else {
      ADD_INSTR(move, MImv, ret_val_reg, getIRegister(10));
    }
    ADD_INSTR(_, MIret);
    break;
  }
  case VT_FPTOSI: {
    FptosiInst &i = static_cast<FptosiInst &>(ins);
    auto f = GET_VREG(i.getRValue(0));
    ADD_INSTR(fcvtw_s, MIfcvtw_s, f, ins.getName());
    instr_map.insert({&ins, fcvtw_s});
    break;
  }
  case VT_SITOFP: {
    SitofpInst &i = static_cast<SitofpInst &>(ins);
    auto s = GET_VREG(i.getRValue(0));
    ADD_INSTR(fcvts_w, MIfcvts_w, s, ins.getName());
    instr_map.insert({&ins, fcvts_w});
    break;
  }
  case VT_ALLOCA: {
    AllocaInst &alloca = static_cast<AllocaInst &>(ins);
    auto tp = alloca.getType();
    auto size = cal_size(tp);
    ADD_INSTR(_, MIaddiw, &reg_sp, size, &reg_sp);
    ADD_INSTR(move, MImv, &reg_sp, ins.getName());
    instr_map.insert({&ins, move});
    break;
  }
  case VT_LOAD: {
    // depend on loaded type and
    // 1. load pointer(get pointer through alloca, no heap alloaction)
    // 2. load global variale
    LoadInst &load = static_cast<LoadInst &>(ins);
    auto tp = load.getType();
    auto addr = load.getRValue(0);
    if (addr->getValueTag() == VT_GLOBALVAR) {
      auto g = global_map.at(static_cast<GlobalVariable *>(addr));
      if (tp == Type::getFloatType()) {
        ADD_INSTR(mload, MIflw, g);
        instr_map.insert({&ins, mload});
      } else {
        ADD_INSTR(mload, MIlw, g);
        instr_map.insert({&ins, mload});
      }
    } else {
      auto a = GET_VREG(addr);
      if (tp == Type::getFloatType()) {
        ADD_INSTR(mload, MIflw, a, ins.getName());
        instr_map.insert({&ins, mload});
      } else {
        ADD_INSTR(mload, MIlw, a, ins.getName());
        instr_map.insert({&ins, mload});
      }
    }

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

    auto v = GET_VREG(value);
    if (addr->getValueTag() == VT_GLOBALVAR) {
      auto g = global_map.at(static_cast<GlobalVariable *>(addr));
      if (value->getType() == Type::getFloatType()) {
        ADD_INSTR(_, MIfsw, g, v);
      } else {
        ADD_INSTR(_, MIsw, g, v);
      }
    } else {
      auto a = GET_VREG(addr);
      if (value->getType() == Type::getFloatType()) {
        ADD_INSTR(_, MIfsw, a, v);
      } else {
        ADD_INSTR(_, MIsw, a, v);
      }
    }
    break;
  }
  case VT_GEP: {
    // calculate address
    GetElemPtrInst &gep = static_cast<GetElemPtrInst &>(ins);

    Register *base = GET_VREG(gep.getRValue(0));

    const Type *current_type = gep.getPtrType();
    MachineInstruction *dest;
    for (unsigned i = 1; i < gep.getRValueSize(); i++) {
      Register *index = GET_VREG(gep.getRValue(i));

      ADD_INSTR(elesz, MIli, cal_size(current_type));
      ADD_INSTR(offset, MImulw, index, elesz);
      ADD_INSTR(addr, MIaddw, base, offset);

      if (i != gep.getRValueSize() - 1) {
        current_type =
            static_cast<const ArrayType *>(current_type)->getElemType();
      }
      dest = addr;
    }
    dest->setName(ins.getName());
    instr_map.insert({&ins, dest});
    break;
  }
  case VT_PHI: {
    PhiInst &phi = static_cast<PhiInst &>(ins);
    // 遍历 Phi 指令的所有操作数
    ADD_INSTR(mphi, MIphi, phi.getName());
    for (int i = 0; i < phi.getRValueSize(); i += 2) {
      Value *opd = phi.getRValue(i);
      BasicBlock *pred_bb = static_cast<BasicBlock *>(phi.getRValue(i + 1));
      auto *mvreg = GET_VREG(opd);
      auto *mbb = bb_map.at(pred_bb);
      mphi->pushIncoming(mvreg, mbb);
    }
    instr_map.insert({&ins, mphi});
    break;
  }
  case VT_ZEXT: {
    ZextInst &zt = static_cast<ZextInst &>(ins);
    Register *oprand = GET_VREG(zt.getRValue(0));
    ADD_INSTR(move, MImv, oprand, ins.getName());
    instr_map.insert({&ins, move});
    break;
  }
  case VT_ICMP: {
    IcmpInst &icmp = static_cast<IcmpInst &>(ins);
    auto opd1 = icmp.getRValue(0);
    auto opd2 = icmp.getRValue(1);
    switch (icmp.getOpTag()) {
    case EQ: {
      ADD_INSTR(sub_ins, MIsubw, GET_VREG(opd1), GET_VREG(opd2));
      ADD_INSTR(eq_zero, MIsltiu, sub_ins, 1, ins.getName());
      instr_map.insert({&ins, eq_zero});
      break;
    }
    case NE: {
      ADD_INSTR(sub_ins, MIsubw, GET_VREG(opd1), GET_VREG(opd2));
      ADD_INSTR(sltu_ins, MIsltu, &reg_zero, sub_ins, ins.getName());
      instr_map.insert({&ins, sltu_ins});
      break;
    }
    case SLE: {
      ADD_INSTR(slt_ins, MIslt, GET_VREG(opd2), GET_VREG(opd1));
      ADD_INSTR(snot_ins, MInot, slt_ins, ins.getName());
      instr_map.insert({&ins, snot_ins});
      break;
    }

      BINARY_INSTR_WITH_IMM_CASE(SLT, MIslt, MIslti, IntegerConstant)

    case SGE: {
      CREATE_BINARY_INSTR_WITH_IMM(slt_ins, MIslt, MIslti, opd1, opd2,
                                   IntegerConstant)
      ADD_INSTR(sge_ins, MInot, slt_ins, ins.getName());
      instr_map.insert({&ins, sge_ins});
      break;
    }
    case SGT: {
      // 对于SGT（Signed Greater Than），我们可以使用SLT并交换操作数
      CREATE_BINARY_INSTR_WITH_IMM(slt_ins, MIslt, MIslti, opd2, opd1,
                                   IntegerConstant)
      slt_ins->setName(ins.getName());
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
      BINARY_OP_CASE(OEQ, MIfeq_s)
    case ONE: {
      ADD_INSTR(feq, MIfeq_s, GET_VREG(opd1), GET_VREG(opd2), ins.getName());
      ADD_INSTR(ne, MInot, feq, ins.getName());
      instr_map.insert({&ins, ne});
      break;
    }
      BINARY_OP_CASE(OLT, MIflt_s)
      BINARY_OP_CASE(OLE, MIfle_s)
      BINARY_OP_REVERSE(OGT, MIflt_s)
      BINARY_OP_REVERSE(OGE, MIfle_s)
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

      BINARY_INSTR_WITH_IMM_CASE(ADD, MIaddw, MIaddiw, IntegerConstant)
      BINARY_INSTR_WITH_IMM_CASE(AND, MIand, MIandi, IntegerConstant)
      BINARY_INSTR_WITH_IMM_CASE(OR, MIor, MIori, IntegerConstant)
      BINARY_INSTR_WITH_IMM_CASE(XOR, MIxor, MIxori, IntegerConstant)
      BINARY_OP_CASE(SUB, MIsubw)
      BINARY_OP_CASE(MUL, MImulw)
      BINARY_OP_CASE(SDIV, MIdivw)
      BINARY_OP_CASE(SREM, MIremw)
      BINARY_OP_CASE(FADD, MIfadd_s)
      BINARY_OP_CASE(FSUB, MIfsub_s)
      BINARY_OP_CASE(FMUL, MIfmul_s)
      BINARY_OP_CASE(FDIV, MIfdiv_s)

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

  for (const auto &func : ir->getFunctions()) {
    MachineFunction *mfunc = res->addFunction(
        static_cast<FuncType *>(func->getType()), func->getName());
        
    func_map->insert({&*func, mfunc});
  }

  for (const auto &func : ir->getFunctions()) {
    MachineFunction *mfunc = func_map->at(&*func);

    for (const auto &bb : func->getBasicBlocks()) {
      MachineBasicBlock *mbb = res->addBasicBlock(mfunc, bb->getName());
      bb_map->insert({&*bb, mbb});
    }

    for (const auto &bb : func->getBasicBlocks()) {
      MachineBasicBlock *mbb = bb_map->at(&*bb);
      for (const auto &i : bb->getInstructions()) {
        auto minstrs = select_instruction(res, *i, *instr_map, *bb_map,
                                          *func_map, *global_map, &*func);
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
