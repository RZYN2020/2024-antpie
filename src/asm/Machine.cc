#include "asm/Machine.hh"


/////////////////////////////////////////////////
//
//                MachineGlobal
//
/////////////////////////////////////////////////

void MachineGlobal::printASM(ostream& stream) const {
    // todo
}

/////////////////////////////////////////////////
//
//                MachineBasicBlock
//
/////////////////////////////////////////////////


void MachineBasicBlock::printASM(ostream& stream) const {
    // do some thing
    for (const auto& instr : *instructions) {
    stream << "  ";
    instr->printASM(stream);
    stream << endl;
  }
}

/////////////////////////////////////////////////
//
//                MachineFunction
//
/////////////////////////////////////////////////

MachineFunction::MachineFunction(FuncType* fType, string name) {
  //todo
}

void MachineFunction::pushBasicBlock(MachineBasicBlock* bb) {
  //todo
}

void MachineFunction::printASM(ostream& stream) const {
    // do some thing
    for (const auto& bb : *basicBlocks) {
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

void MachineModule::printASM(ostream& stream) const {
  for (const auto& gv : *globalVariables) {
    gv->printASM(stream);
    stream << endl;
  }
  stream << endl;
  for (const auto& f : *functions) {
    f->printASM(stream);
    stream << endl;
  }
}