#include "Machine.hh"

/////////////////////////////////////////////////
//
//                MachineBasicBlocks
//
/////////////////////////////////////////////////

string MachineBasicBlock::to_string() const {
  string res = getName() + ":\n";
  for (auto &ins : *instructions) {
    res += "\t" +ins->to_string() + "\n";
  }
  res += "\n";
  return res;
}

/////////////////////////////////////////////////
//
//                MachineGlobal
//
/////////////////////////////////////////////////

static void add_decl(string& res, Constant* init, Type* tp) {
  switch (tp->getTypeTag())
  {
  case TT_INT32:
    res += "\t.word 0\n";
    break;
  case TT_INT1:
    res +=  "\t.word 0\n";
    break;
  case TT_FLOAT:
    res +=  "\t.word 0\n";
    break;
  case TT_ARRAY:
    res +=  "\t.word 0\n";
    break;
  case TT_POINTER:
    res +=  "\t.word 0\n";
    break;
    // return add_decl(res, init, static_cast<PointerType*>(tp)->getElemType());
  default:
    res +=  "\t.word 0\n";
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