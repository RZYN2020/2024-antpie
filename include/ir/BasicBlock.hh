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

 public:
  BasicBlock(string name_) : Value(nullptr, name_, VT_BB), tail(nullptr), empty(false) {}
  BasicBlock(string name_, bool empty_) : Value(nullptr, name_, VT_BB), tail(nullptr), empty(empty_) {}
  ~BasicBlock();
  void pushInstr(Instruction* i);
  void printIR(ostream& stream) const override;
  Instruction* getTailInstr() { return tail; }

  const LinkedList<Instruction*>* getInstructions() const {
    return &instructions;
  }

  bool isEmpty() { return empty; }
};

#endif