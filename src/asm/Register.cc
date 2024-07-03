#include "Register.hh"
#include "MachineInstruction.hh"

// void Register::replaceRegisterUsers(Register *newReg) {
//   for (MachineInstruction *use : uses) {
//     use->replaceRegister(this, newReg);
//   }
//   uses.clear();
//   newReg->uses = this->uses;
// }