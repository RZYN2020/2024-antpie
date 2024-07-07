#include "BasicBlock.hh"

#include "Function.hh"

BasicBlock::~BasicBlock() {
  for (const auto& instr : instructions) {
    delete instr;
  }
}

void BasicBlock::printIR(ostream& stream) const {
  stream << getName() << ":" << endl;
  for (const auto& instr : instructions) {
    stream << "  ";
    instr->printIR(stream);
    stream << endl;
  }
}

bool isTail(Instruction* instr) {
  return instr->getValueTag() == VT_BR || instr->getValueTag() == VT_RET ||
         instr->getValueTag() == VT_JUMP;
}

void BasicBlock::pushInstr(Instruction* instr) {
  instr->setParent(this);
  instructions.pushBack(instr);
  if (isTail(instr)) {
    if (tail) {
      // std::cout << "Multiple jump instruction in bb";
    }
    tail = instr;
  }
}

void BasicBlock::pushInstrAtHead(Instruction* instr) {
  instr->setParent(this);
  instructions.pushFront(instr);
}

void BasicBlock::eraseFromParent() {
  getParent()->getBasicBlocks()->remove(this);
  function = nullptr;
}
