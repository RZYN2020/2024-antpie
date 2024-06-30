#include "Function.hh"
#include "CFG.hh"

Function::Function(FuncType* fType, string name)
    : GlobalValue(fType, name, VT_FUNC) {}

Function::~Function() {
  for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
    delete *it;
  }
  delete cfg;
}

void Function::pushBasicBlock(BasicBlock* bb) { basicBlocks.pushBack(bb); }

void Function::pushBasicBlockAtHead(BasicBlock* bb) {
  basicBlocks.pushFront(bb);
}

void Function::printIR(ostream& stream) const {
  FuncType* funcType = dynamic_cast<FuncType*>(getType());
  stream << "define dso_local " << funcType->getRetType()->toString() << " @"
         << getName() << "(";
  int argSize = funcType->getArgSize();
  for (int i = 0; i < argSize; i++) {
    if (i != 0) {
      stream << ", ";
    }
    stream << funcType->getArgument(i)->getType()->toString() << " "
           << funcType->getArgument(i)->toString();
  }
  stream << ") {" << endl;

  for (auto bb = basicBlocks.begin(); bb != basicBlocks.end(); ++bb) {
    if ((*bb)->isEmpty()) continue;
    (*bb)->printIR(stream);
    stream << endl;
  }
  stream << "}" << endl;
}

CFG* Function::buildCFG() {

  entry = new BasicBlock(getName() + "_entry", true);
  exit = new BasicBlock(getName() + "_exit", true);
  pushBasicBlock(exit);
  pushBasicBlockAtHead(entry);
  cfg = new CFG(this);
  return cfg;
}