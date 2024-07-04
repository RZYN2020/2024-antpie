#include "Machine.hh"

/////////////////////////////////////////////////
//
//                MachineBasicBlocks
//
/////////////////////////////////////////////////

string MachineBasicBlock::to_string() const {
  string res = getName() + ":\n";
  for (auto &ins : *instructions) {
    // std::cout << "      " + ins->to_string() << std::endl;
    res += "\t" + ins->to_string() + "\n";
  }
  res += "\n";
  return res;
}
MachineBasicBlock::MachineBasicBlock(string name_) {
  name = name_;
  instructions = make_unique<vector<unique_ptr<MachineInstruction>>>();
}
void MachineBasicBlock::pushInstr(MachineInstruction *i) {
  instructions->push_back(unique_ptr<MachineInstruction>(i));
  i->setBasicBlock(this);
}

void MachineBasicBlock::pushInstrs(vector<MachineInstruction *> is) {
  for (auto i : is) {
    instructions->push_back(unique_ptr<MachineInstruction>(i));
    i->setBasicBlock(this);
  }
}

void MachineBasicBlock::pushInstrBeforeJmp(MachineInstruction *i) {
  for (auto it = instructions->begin(); it != instructions->end(); ++it) {
    auto tg = (*it)->getTag();
    if (tg == MachineInstruction::J || tg == MachineInstruction::BEQ ||
        tg == MachineInstruction::RET) {
      it = instructions->insert(it, unique_ptr<MachineInstruction>(i));
      i->setBasicBlock(this);
      return;
    }
  }
}

void MachineBasicBlock::pushInstrAtHead(MachineInstruction *i) {
  instructions->insert(instructions->begin(),
                       unique_ptr<MachineInstruction>(i));
  i->setBasicBlock(this);
}

void MachineBasicBlock::pushInstrsAtHead(vector<MachineInstruction *> is) {
  for (int i = is.size() - 1; i >= 0; i--) {
    pushInstrAtHead(is.at(i));
  }
}

void MachineBasicBlock::setFunction(MachineFunction *function) {
  this->function = function;
}

MachineFunction *MachineBasicBlock::getFunction() { return function; }

unique_ptr<MachineInstruction>
MachineBasicBlock::removeInstruction(MachineInstruction *ins) {
  for (auto it = instructions->begin(); it != instructions->end();) {
    if (it->get() == ins) {
      unique_ptr<MachineInstruction> removed = std::move(*it);
      it = instructions->erase(it);
      removed->setBasicBlock(nullptr);
      return removed;
    } else {
      ++it;
    }
  }
  return nullptr;
}

void MachineBasicBlock::replaceInstructionWith(
    MachineInstruction *ins, vector<MachineInstruction *> instrs) {
  for (auto it = instructions->begin(); it != instructions->end(); ++it) {
    if (it->get() == ins) {
      instructions->erase(it);
      ins->setBasicBlock(nullptr);

      for (auto new_ins : instrs) {
        instructions->insert(it, unique_ptr<MachineInstruction>(new_ins));
        new_ins->setBasicBlock(this);
      }
      return;
    }
  }
}

void MachineBasicBlock::insertBeforeInstructionWith(
    MachineInstruction *ins, vector<MachineInstruction *> instrs) {
  for (auto it = instructions->begin(); it != instructions->end(); ++it) {
    if (it->get() == ins) {
      for (auto new_ins : instrs) {
        it = instructions->insert(it, unique_ptr<MachineInstruction>(new_ins));
        new_ins->setBasicBlock(this);
        ++it;
      }
      return;
    }
  }
}

void MachineBasicBlock::insertAfterInstructionWith(
    MachineInstruction *ins, vector<MachineInstruction *> instrs) {
  for (auto it = instructions->begin(); it != instructions->end(); ++it) {
    if (it->get() == ins) {
      ++it;
      for (auto new_ins : instrs) {
        it = instructions->insert(it, unique_ptr<MachineInstruction>(new_ins));
        new_ins->setBasicBlock(this);
        ++it;
      }
      return;
    }
  }
}

/////////////////////////////////////////////////
//
//                MachineGlobal
//
/////////////////////////////////////////////////

// in bytes
uint32_t cal_size(const Type *tp) {
  switch (tp->getTypeTag()) {
  case TT_POINTER:
  case TT_INT1:
  case TT_INT32:
  case TT_FLOAT:
    return 4;
  case TT_ARRAY: {
    const ArrayType *atp = static_cast<const ArrayType *>(tp);
    return atp->getLen() * cal_size(atp->getElemType());
  }
  case TT_FUNCTION:
  case TT_VOID:
  default:
    assert(0);
  }
  return 0;
}

union FloatIntUnion {
  float f;
  int32_t i;
};

int32_t float_to_int_bits(float f) {
  FloatIntUnion u;
  u.f = f;
  return u.i;
}

static void add_decl(string &res, Constant *init, Type *tp) {
  switch (tp->getTypeTag()) {
  case TT_INT32: {
    res += "\t.word " + init->toString() + "\n";
    break;
  }
  case TT_INT1:
    res += "\t.word " + init->toString() + "\n";
    break;
  case TT_FLOAT: {
    auto f = static_cast<FloatConstant *>(init);
    res += "\t.word " + std::to_string(float_to_int_bits(f->getValue())) + "\n";
    break;
  }
  case TT_ARRAY: {
    ArrayType *arrType = static_cast<ArrayType *>(tp);
    Type *elemType = arrType->getElemType();
    int size = arrType->getLen();

    if (init->isZeroInit()) {
      res += "\t.zero " + std::to_string(size * cal_size(elemType)) + "\n";
      break;
    }

    auto arrInit = static_cast<ArrayConstant *>(init);
    for (int i = 0; i < size; i++) {
      Constant *elemInit = arrInit->getElemInit(i);
      add_decl(res, elemInit, elemType);
    }
    break;
  }
  case TT_POINTER: {
    PointerType *ptrType = static_cast<PointerType *>(tp);
    Type *elemType = ptrType->getElemType();
    if (init->isZeroInit()) {
      res += "\t.word 0\n";
    } else {
      add_decl(res, init, elemType);
    }
    break;
  }
  default:
    res += "\t.word 0\n";
    break;
  }
}

string MachineGlobal::to_string() const {
  string res = getName() + ":\n";
  auto tp = global->getType();
  auto init = global->getInitValue();
  add_decl(res, init, tp);
  return res;
}

/////////////////////////////////////////////////
//
//                MachineFunction
//
/////////////////////////////////////////////////

MachineFunction::MachineFunction(FuncType *fType, string name_) {
  type = fType;
  name = name_;
  basicBlocks = make_unique<vector<unique_ptr<MachineBasicBlock>>>();
  reg_pool = make_unique<vector<unique_ptr<MachineInstruction>>>();
  saved_registers = make_unique<vector<Register *>>();
}

void MachineFunction::pushBasicBlock(MachineBasicBlock *bb) {
  basicBlocks->push_back(unique_ptr<MachineBasicBlock>(bb));
  bb->setFunction(this);
}

string MachineFunction::to_string() const {
  string res = getName() + ":\n";
  // std::cout << res << std::endl;
  for (const auto &bb : *basicBlocks) {
    // std::cout << "  bb:" << std::endl;
    // std::cout << "  :" + bb->getName() << std::endl;
    res += bb->to_string();
  }
  return res;
}

/////////////////////////////////////////////////
//
//                MachineModule
//
/////////////////////////////////////////////////

MachineModule::MachineModule() {
  globalVariables = make_unique<vector<unique_ptr<MachineGlobal>>>();
  functions = make_unique<vector<unique_ptr<MachineFunction>>>();
}

MachineFunction *MachineModule::addFunction(FuncType *funcType, string name) {
  MachineFunction *func = new MachineFunction(funcType, name);
  functions->push_back(unique_ptr<MachineFunction>(func));
  return func;
}

MachineBasicBlock *MachineModule::addBasicBlock(MachineFunction *function,
                                                string name) {
  MachineBasicBlock *bb = new MachineBasicBlock(name);
  function->pushBasicBlock(bb);
  return bb;
}

MachineGlobal *MachineModule::addGlobalVariable(GlobalVariable *global) {
  auto g = new MachineGlobal(global);
  globalVariables->push_back(unique_ptr<MachineGlobal>(g));
  return g;
}

MachineGlobal *MachineModule::addGlobalFloat(FloatConstant *f) {
  static int float_cnt = 0;
  auto g = new MachineGlobal(
      new GlobalVariable(FloatType::getFloatType(), f, "fi" + float_cnt));
  globalVariables->push_back(unique_ptr<MachineGlobal>(g));
  return g;
}

string MachineModule::to_string() const {
  string res = ".globl main\n";
  for (const auto &gv : *globalVariables) {
    res += gv->to_string() + "\n";
  }
  for (const auto &f : *functions) {
    res += f->to_string();
  }
  return res;
}

IRegister *Register::reg_zero = new IRegister(0, "zero");
IRegister *Register::reg_ra = new IRegister(1, "ra"); // saved
IRegister *Register::reg_sp = new IRegister(2, "sp"); // saved
IRegister *Register::reg_gp = new IRegister(3, "gp");
IRegister *Register::reg_tp = new IRegister(4, "tp");
IRegister *Register::reg_t0 = new IRegister(5, "t0");
IRegister *Register::reg_t1 = new IRegister(6, "t1");
IRegister *Register::reg_t2 = new IRegister(7, "t2");
IRegister *Register::reg_s0 =
    new IRegister(8, "s0"); // 注意：s0 和 fp 是同一个寄存器
IRegister *Register::reg_s1 = new IRegister(9, "s1");
IRegister *Register::reg_a0 = new IRegister(10, "a0");
IRegister *Register::reg_a1 = new IRegister(11, "a1");
IRegister *Register::reg_a2 = new IRegister(12, "a2");
IRegister *Register::reg_a3 = new IRegister(13, "a3");
IRegister *Register::reg_a4 = new IRegister(14, "a4");
IRegister *Register::reg_a5 = new IRegister(15, "a5");
IRegister *Register::reg_a6 = new IRegister(16, "a6");
IRegister *Register::reg_a7 = new IRegister(17, "a7");
IRegister *Register::reg_s2 = new IRegister(18, "s2");
IRegister *Register::reg_s3 = new IRegister(19, "s3");
IRegister *Register::reg_s4 = new IRegister(20, "s4");
IRegister *Register::reg_s5 = new IRegister(21, "s5");
IRegister *Register::reg_s6 = new IRegister(22, "s6");
IRegister *Register::reg_s7 = new IRegister(23, "s7");
IRegister *Register::reg_s8 = new IRegister(24, "s8");
IRegister *Register::reg_s9 = new IRegister(25, "s9");
IRegister *Register::reg_s10 = new IRegister(26, "s10");
IRegister *Register::reg_s11 = new IRegister(27, "s11");
IRegister *Register::reg_t3 = new IRegister(28, "t3");
IRegister *Register::reg_t4 = new IRegister(29, "t4");
IRegister *Register::reg_t5 = new IRegister(30, "t5");
IRegister *Register::reg_t6 = new IRegister(31, "t6");

Register *Register::getIRegister(int idx) {
  static IRegister *iregisters[] = {
      Register::reg_zero, Register::reg_ra, Register::reg_sp,
      Register::reg_gp,   Register::reg_tp, Register::reg_t0,
      Register::reg_t1,   Register::reg_t2, Register::reg_s0,
      Register::reg_s1,   Register::reg_a0, Register::reg_a1,
      Register::reg_a2,   Register::reg_a3, Register::reg_a4,
      Register::reg_a5,   Register::reg_a6, Register::reg_a7,
      Register::reg_s2,   Register::reg_s3, Register::reg_s4,
      Register::reg_s5,   Register::reg_s6, Register::reg_s7,
      Register::reg_s8,   Register::reg_s9, Register::reg_s10,
      Register::reg_s11,  Register::reg_t3, Register::reg_t4,
      Register::reg_t5,   Register::reg_t6,
  };

  if (idx < 0 || idx >= (sizeof(iregisters) / sizeof(iregisters[0]))) {
    throw std::out_of_range("Index is out of the range of IRegister array.");
  }
  return iregisters[idx];
}

FRegister *Register::reg_ft0 = new FRegister(0, "ft0");
FRegister *Register::reg_ft1 = new FRegister(1, "ft1");
FRegister *Register::reg_ft2 = new FRegister(2, "ft2");
FRegister *Register::reg_ft3 = new FRegister(3, "ft3");
FRegister *Register::reg_ft4 = new FRegister(4, "ft4");
FRegister *Register::reg_ft5 = new FRegister(5, "ft5");
FRegister *Register::reg_ft6 = new FRegister(6, "ft6");
FRegister *Register::reg_ft7 = new FRegister(7, "ft7");
FRegister *Register::reg_fs0 = new FRegister(8, "fs0");
FRegister *Register::reg_fs1 = new FRegister(9, "fs1");
FRegister *Register::reg_fa0 = new FRegister(10, "fa0");
FRegister *Register::reg_fa1 = new FRegister(11, "fa1");
FRegister *Register::reg_fa2 = new FRegister(12, "fa2");
FRegister *Register::reg_fa3 = new FRegister(13, "fa3");
FRegister *Register::reg_fa4 = new FRegister(14, "fa4");
FRegister *Register::reg_fa5 = new FRegister(15, "fa5");
FRegister *Register::reg_fa6 = new FRegister(16, "fa6");
FRegister *Register::reg_fa7 = new FRegister(17, "fa7");
FRegister *Register::reg_fs2 = new FRegister(18, "fs2");
FRegister *Register::reg_fs3 = new FRegister(19, "fs3");
FRegister *Register::reg_fs4 = new FRegister(20, "fs4");
FRegister *Register::reg_fs5 = new FRegister(21, "fs5");
FRegister *Register::reg_fs6 = new FRegister(22, "fs6");
FRegister *Register::reg_fs7 = new FRegister(23, "fs7");
FRegister *Register::reg_fs8 = new FRegister(24, "fs8");
FRegister *Register::reg_fs9 = new FRegister(25, "fs9");
FRegister *Register::reg_fs10 = new FRegister(26, "fs10");
FRegister *Register::reg_fs11 = new FRegister(27, "fs11");
FRegister *Register::reg_ft8 = new FRegister(28, "ft8");
FRegister *Register::reg_ft9 = new FRegister(29, "ft9");
FRegister *Register::reg_ft10 = new FRegister(30, "ft10");
FRegister *Register::reg_ft11 = new FRegister(31, "ft11");

Register *Register::getFRegister(int idx) {
  static FRegister *fregisters[] = {
      Register::reg_ft0,  Register::reg_ft1,  Register::reg_ft2,
      Register::reg_ft3,  Register::reg_ft4,  Register::reg_ft5,
      Register::reg_ft6,  Register::reg_ft7,  Register::reg_fs0,
      Register::reg_fs1,  Register::reg_fa0,  Register::reg_fa1,
      Register::reg_fa2,  Register::reg_fa3,  Register::reg_fa4,
      Register::reg_fa5,  Register::reg_fa6,  Register::reg_fa7,
      Register::reg_fs2,  Register::reg_fs3,  Register::reg_fs4,
      Register::reg_fs5,  Register::reg_fs6,  Register::reg_fs7,
      Register::reg_fs8,  Register::reg_fs9,  Register::reg_fs10,
      Register::reg_fs11, Register::reg_ft8,  Register::reg_ft9,
      Register::reg_ft10, Register::reg_ft11,
  };

  if (idx < 0 || idx >= (sizeof(fregisters) / sizeof(fregisters[0]))) {
    throw std::out_of_range("Index is out of the range of FRegister array.");
  }
  return fregisters[idx];
}
