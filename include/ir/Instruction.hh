/**
 * Instruction: subclass of value
 * Create by Zhang Junbin at 2024/6/1
 */

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <memory>
#include <vector>

#include "Value.hh"

using std::make_unique;
using std::unique_ptr;
using std::vector;

class Instruction : public Value {
 private:
  unique_ptr<vector<Use*>> useList;
  unique_ptr<vector<Value*>> valueList;

 public:
  Instruction() : Value(nullptr, VT_INSTR) {
    useList = make_unique<vector<Use*>>();
    valueList = make_unique<vector<Value*>>();
  }
};

class AllocaInst : public Instruction {
 public:
  AllocaInst(Type* type);
};

class BinaryOpInst : public Instruction {
 private:
  // enum BinaryOp bOpType;
 public:
  BinaryOpInst(int opType, Value* op1, Value* op2, string name);
};

class BranchInst : public Instruction {
 public:
  // BranchInst(Value* cond, BasicBlock* trueBlock, BasicBlock* falseBlack);
};

class CallInst : public Instruction {
 public:
  // CallInst(Function* func, vector<Value*>& params, string name);
};

class CmpInst : public Instruction {
 private:
  // enum CmpOp cmpType;
 public:
  CmpInst(int opType, Value* op1, Value* op2, string name);
};

class FptosiInst : public Instruction {
 public:
  FptosiInst(Value* src);
};

class GetElemPtrInst : public Instruction {
 public:
  GetElemPtrInst(Type* ptrType, Value* ptr, int idx1, int idx2);
};

class JumpInst : public Instruction {
 public:
  // JumpInst(BasicBlock* block);
};

class LoadInst : public Instruction {
 public:
  LoadInst(Value* addr, string name);
};

class PhiInst : public Instruction {
 public:
  PhiInst(string name);
};

class ReturnInst : public Instruction {
 public:
  ReturnInst(Value* retValue);
};

class SitofpInst : public Instruction {
 public:
  SitofpInst(Value* src);
};

class StoreInst : public Instruction {
 public:
  StoreInst(Value* value, Value* add);
};

class UnaryOpInst : public Instruction {
 private:
  // enum UnaryOp uOpType;
 public:
  UnaryOpInst(int opType, Value* op1, string name);
};

#endif