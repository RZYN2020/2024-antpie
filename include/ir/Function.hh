/**
 * Function ir
 * Create by Zhang Junbin at 2024/6/2
 */

#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "BasicBlock.hh"
#include "Type.hh"
#include "Value.hh"

class Function : public GlobalValue {
 private:
  unique_ptr<vector<unique_ptr<BasicBlock>>> basicBlocks;

 public:
  Function(FuncType* fType, string name);
  void pushBasicBlock(BasicBlock* bb);
  void printIR(ostream& stream) const override;
};

#endif