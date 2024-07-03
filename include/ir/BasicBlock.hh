/**
 * BasicBlock ir
 * Create by Zhang Junbin at 2024/6/2
 */
#ifndef _BASIC_BLOCK_H
#define _BASIC_BLOCK_H

#include "Instruction.hh"
#include "LinkedList.hh"
#include "Value.hh"

class BasicBlock : public Value {
 private:
  LinkedList<Instruction*> instructions;
  Instruction* tail;
  bool empty;
  Function* function;

 public:
  BasicBlock(string name_)
      : Value(nullptr, name_, VT_BB), tail(nullptr), empty(false) {}
  BasicBlock(string name_, bool empty_)
      : Value(nullptr, name_, VT_BB), tail(nullptr), empty(empty_) {}
  ~BasicBlock();
  void pushInstr(Instruction* i);
  void pushInstrAtHead(Instruction* i);
  void printIR(ostream& stream) const override;
  Instruction* getTailInstr() { return tail; }
  Function* getParent() { return function; }
  void setParent(Function* func) { function = func; }

  const LinkedList<Instruction*>* getInstructions() const {
    return &instructions;
  }
  LinkedList<Instruction*>* getInstructions() { return &instructions; }

  bool isEmpty() { return empty; }
};

#endif