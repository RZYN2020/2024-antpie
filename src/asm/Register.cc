#include "Register.hh"
#include "MachineInstruction.hh"

void VRegister::replaceVRegisterUsers(VRegister *newVReg) {
  for (VRegister *use : uses) {
    MachineInstruction *inst = static_cast<MachineInstruction *>(use);
    inst->replaceVRegister(this, newVReg);
  }
  uses.clear();
  newVReg->uses = this->uses;
}