#include "MachineInstruction.hh"

void MachineInstruction::pushReg(Register *r) {}

void MachineInstruction::setImm(Immediate i) {}

MachineInstruction::MachineInstructionTag MachineInstruction::getTag() const {}

Register *MachineInstruction::getReg(int idx) const {}

Immediate *MachineInstruction::getImm() const {}

void MachineInstruction::printASM(ostream &stream) const {}