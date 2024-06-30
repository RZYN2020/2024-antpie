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

void MachineInstruction::pushJTarget(MachineBasicBlock *b) {
  j_targets->push_back(b);
}

void MachineInstruction::setImm(Immediate i) {
  imm = make_unique<Immediate>(i);
}

void MachineInstruction::setFunction(MachineFunction *fun_) { fun = fun_; }

MachineInstruction::MachineInstructionTag MachineInstruction::getTag() const {
  return tag;
}

Register *MachineInstruction::getReg(int idx) const { return oprands->at(idx); }

MachineBasicBlock *MachineInstruction::getJTarget(int idx) const {
  return j_targets->at(idx);
}

Immediate *MachineInstruction::getImm() const { return &*imm; }

void MachineInstruction::printASM(ostream &stream) const {}

void MachineInstruction::setGlobal(MachineGlobal *global_) { global = global_; }

MachineGlobal *MachineInstruction::getGlobal() const { return global; }

MachineFunction *MachineInstruction::getFunction() const { return fun; }

void MachineInstruction::setTarget(Register *reg) { target = reg; }

Register *MachineInstruction::getTarget(Register *reg) {
  if (target == nullptr) {
    return this;
  } else {
    return target;
  }
}
