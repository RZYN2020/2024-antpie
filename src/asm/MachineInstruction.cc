#include "MachineInstruction.hh"

/////////////////////////////////////////////////
//
//                MachineBasicBlock
//
/////////////////////////////////////////////////

void MachineBasicBlock::printASM(ostream &stream) const {
  // do some thing
  for (const auto &instr : *instructions) {
    stream << "  ";
    instr->printASM(stream);
    stream << endl;
  }
}


/////////////////////////////////////////////////
//
//                MachineInstruction
//
/////////////////////////////////////////////////

void MachineInstruction::pushReg(Register *r) { oprands->push_back(r); }

void MachineInstruction::pushTarget(MachineBasicBlock *b) { targets->push_back(b); }

void MachineInstruction::setImm(Immediate i) {
  imm = make_unique<Immediate>(i);
}

MachineInstruction::MachineInstructionTag MachineInstruction::getTag() const {
  return tag;
}

Register *MachineInstruction::getReg(int idx) const { return oprands->at(idx); }

MachineBasicBlock *MachineInstruction::getTarget(int idx) const {
  return targets->at(idx);
}

Immediate *MachineInstruction::getImm() const { return &*imm; }

void MachineInstruction::printASM(ostream &stream) const {}

void MachineInstruction::setGlobal(MachineGlobal *global_) { global = global_; }

MachineGlobal *MachineInstruction::getGlobal() const { return global; }
