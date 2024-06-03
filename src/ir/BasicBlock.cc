#include "ir/BasicBlock.hh"

void BasicBlock::printIR(ostream& stream) const {
  stream << getName() << ":" << endl;
  for (const auto& instr : *instructions) {
    stream << "  ";
    instr->printIR(stream);
    stream << endl;
  }
}
