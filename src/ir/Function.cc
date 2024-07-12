#include "Function.hh"

#include "CFG.hh"

Function::Function(FuncType* fType, string name)
    : GlobalValue(fType, name, VT_FUNC) {
  entry = new BasicBlock(name + "_entry");
  pushBasicBlock(entry);
}

Function::~Function() {
  for (const auto& bb : basicBlocks) {
    delete bb;
  }
  delete cfg;
}

void Function::pushBasicBlock(BasicBlock* bb) {
  bb->setParent(this);
  if (basicBlocks.getSize() == 1) {
    entry->pushInstr(new JumpInst(bb));
  }
  basicBlocks.pushBack(bb);
}

void Function::pushBasicBlockAtHead(BasicBlock* bb) {
  bb->setParent(this);
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

  for (const auto& bb : basicBlocks) {
    if (bb->isEmpty()) continue;
    bb->printIR(stream);
    stream << endl;
  }
  stream << "}" << endl;
}

CFG* Function::buildCFG() {
  if (!exit) {
    exit = new BasicBlock(getName() + "_exit", true);
    pushBasicBlock(exit);
  }
  cfg = new CFG(this);
  return cfg;
}

DomTree* Function::buildDT() {
  dt = new DomTree(this);
  dt->buildDomTree();
  return dt;
}

void Function::insertBasicBlockBefore(BasicBlock* newBlock, BasicBlock* loc) {
  newBlock->setParent(this);
  for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
    if (*it == loc) {
      basicBlocks.insertBefore(it, newBlock);
      return;
    }
  }
  assert(0);
}
