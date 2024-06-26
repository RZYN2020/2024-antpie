#include "select_instruction.hh"

void select_instruction(MachineModule *res, ANTPIE::Module *ir) {
  // for(Function f : ir) {
  // register prelude
  //// for basic block in f
  ////// for every instruction -> convert to corresponding machine
  ///instruction(s)
  // register conlusion
  // }
}

vector<MachineInstruction *> select_instruction(Instruction *ins) {
  switch (ins->getValueTag()) {
  case VT_JUMP:
    /* code */
    break;
  case VT_RET:
    /* code */
    break;
  case VT_BR:
    /* code */
    break;
  case VT_CALL:
    /* code */
    break;
  case VT_FPTOSI:
    /* code */
    break;
  case VT_SITOFP:
    /* code */
    break;
  case VT_ALLOCA:
    /* code */
    break;
  case VT_LOAD:
    /* code */
    break;
  case VT_STORE:
    /* code */
    break;
  case VT_GEP:
    /* code */
    break;
  case VT_PHI:
    /* code */
    break;
  case VT_ZEXT:
    /* code */
    break;
  case VT_ICMP:
    /* code */
    break;
  case VT_FCMP:
    /* code */
    break;
  case VT_BOP: {
    BinaryOpInst *bins = static_cast<BinaryOpInst *>(ins);
    // todo: switch on op_type
    break;
  }
  default:
    assert(0);
  }
}

Register *select_oprand(Value *opd) {
  // match opd:
  // argument ->...
  // Instruction(Virtual Register) -> ...
  // BasicBlock -> ...
  // ...
}