/**
 * Function ir
 * Create by Zhang Junbin at 2024/6/2
 */

#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "BasicBlock.hh"
#include "LinkedList.hh"
#include "Type.hh"
#include "Value.hh"

class CFG;

class Function : public GlobalValue {
 private:
  LinkedList<BasicBlock*> basicBlocks;
  CFG* cfg;
  BasicBlock *entry, *exit;


 public:
  Function(FuncType* fType, string name);
  ~Function();
  void pushBasicBlock(BasicBlock* bb);
  void pushBasicBlockAtHead(BasicBlock* bb);
  void printIR(ostream& stream) const override;

  const LinkedList<BasicBlock*>* getBasicBlocks() const { return &basicBlocks; }

  BasicBlock* getEntry() const { return entry; }
  BasicBlock* getExit() const { return exit; }

  CFG* buildCFG();
};

#endif