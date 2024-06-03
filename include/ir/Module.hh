/**
 * IR module
 * Create by Zhang Junbin at 2024/6/2
 */
#ifndef _MODULE_H_
#define _MODULE_H_

#include "Function.hh"
#include "GlobalVariable.hh"

namespace ANTPIE {
class Module {
 private:
  unique_ptr<vector<unique_ptr<GlobalVariable>>> globalVariables;
  unique_ptr<vector<unique_ptr<Function>>> functions;
  BasicBlock* currBasicBlock;

 public:
  Module();
  void pushFunction(Function* function);
  void pushGlobalVariable(GlobalVariable* globalVariable);
  void printIR(ostream& stream) const;
  void setCurrBasicBlock(BasicBlock* bb) { currBasicBlock = bb; }
  void pushInstrution(Instruction* instr) { currBasicBlock->pushInstr(instr); }
};
}  // namespace ANTPIE

#endif