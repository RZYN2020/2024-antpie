#include "Machine.hh"

/////////////////////////////////////////////////
//
//                MachineGlobal
//
/////////////////////////////////////////////////

void MachineGlobal::printASM(ostream &stream) const {
  // todo
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

void MachineFunction::printASM(ostream &stream) const {
  // do some thing
  for (const auto &bb : *basicBlocks) {
    bb->printASM(stream);
    stream << endl;
  }
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
  MachineFunction* func = new MachineFunction(funcType, name);
  functions->push_back(unique_ptr<MachineFunction>(func));
  return func;
}

MachineBasicBlock *MachineModule::addBasicBlock(MachineFunction *function,
                                                string name) {
  MachineBasicBlock* bb = new MachineBasicBlock(name);
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
  auto g = new MachineGlobal(new GlobalVariable(FloatType::getFloatType(), f, "fi" + float_cnt));
  globalVariables->push_back(unique_ptr<MachineGlobal>(g));
  return g;
}

void MachineModule::printASM(ostream &stream) const {
  // todo

  for (const auto &gv : *globalVariables) {
    gv->printASM(stream);
    stream << endl;
  }
  stream << endl;
  for (const auto &f : *functions) {
    f->printASM(stream);
    stream << endl;
  }
}