/**
 * Function ir
 * Create by Zhang Junbin at 2024/6/2
 */

#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "BasicBlock.hh"
#include "Value.hh"
#include "Type.hh"

class Function : public GlobalValue {
 private:
  unique_ptr<vector<BasicBlock*>> basicBlocks;

 public:
  Function(FuncType* fType, string name) : GlobalValue(fType, name, VT_FUNC) {}
  void printIR(ostream& stream) const override;
};


#endif