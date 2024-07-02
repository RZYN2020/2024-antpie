#include "Machine.hh"

/////////////////////////////////////////////////
//
//                MachineBasicBlocks
//
/////////////////////////////////////////////////

string MachineBasicBlock::to_string() const {
  string res = getName() + ":\n";
  for (auto &ins : *instructions) {
    res += "\t" + ins->to_string() + "\n";
  }
  res += "\n";
  return res;
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
    auto f = static_cast<FloatConstant*>(init);
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
}

void MachineFunction::pushBasicBlock(MachineBasicBlock *bb) {
  basicBlocks->push_back(unique_ptr<MachineBasicBlock>(bb));
}

string MachineFunction::to_string() const {
  string res = getName() + ":\n";
  for (const auto &bb : *basicBlocks) {
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
  reg_pool = make_unique<vector<unique_ptr<VRegister>>>();
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
  string res = "";
  for (const auto &gv : *globalVariables) {
    res += gv->to_string() + "\n";
  }
  for (const auto &f : *functions) {
    res += f->to_string();
  }
  return res;
}