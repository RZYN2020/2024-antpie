/**
 * BasicBlock ir
 * Create by Zhang Junbin at 2024/6/2
 */
#ifndef _BASIC_BLOCK_H
#define _BASIC_BLOCK_H

#include "Instruction.hh"
#include "Value.hh"

class BasicBlock : public Value {
 private:
  unique_ptr<vector<unique_ptr<Instruction>>> instructions;

 public:
  BasicBlock(string name) : Value(nullptr, name, VT_BB) {
    instructions = make_unique<vector<unique_ptr<Instruction>>>();
  }
  void pushInstr(Instruction* i) {
    instructions->push_back(unique_ptr<Instruction>(i));
  }
  void printIR(ostream& stream) const override;
};


#endif