/**
 * Instruction ir
 * Create by Zhang Junbin at 2024/6/1
 */

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "Value.hh"

class BasicBlock;
class Function;

enum OpTag {
  ADD,
  FADD,
  SUB,
  FSUB,
  MUL,
  FMUL,
  SDIV,
  FDIV,
  SREM,
  FREM,
  AND,
  OR,
  XOR,
  EQ,
  NE,
  SLE,
  SLT,
  SGE,
  SGT,
  OEQ,
  ONE,
  OGT,
  OGE,
  OLE,
  OLT,
};

class Instruction : public Value {
 private:
  unique_ptr<vector<Use*>> useList;
  unique_ptr<vector<Value*>> valueList;

 public:
  Instruction(ValueTag vtag);
  Instruction(string name, ValueTag vTag);
  Instruction(Type* t, string name, ValueTag vTag);
  string getOpName(OpTag op) const;
  void pushValue(Value* v);
  Value* getRValue(int idx) const;
  int getRValueSize() const { return valueList->size(); }
};

class AllocaInst : public Instruction {
 private:
  Type* elemType;

 public:
  AllocaInst(Type* type, string name);
  void printIR(ostream& stream) const override;
};

class BinaryOpInst : public Instruction {
 private:
  OpTag bOpType;

 public:
  BinaryOpInst(OpTag opType, Value* op1, Value* op2, string name);
  void printIR(ostream& stream) const override;

  const OpTag getOpTag() const {
    return bOpType;
  }
};

class BranchInst : public Instruction {
 public:
  BranchInst(Value* cond, BasicBlock* trueBlock, BasicBlock* falseBlock);
  void printIR(ostream& stream) const override;
};

class CallInst : public Instruction {
 private:
  Function* function;

 public:
  CallInst(Function* func, string name);
  CallInst(Function* func, vector<Value*>& params, string name);
  void pushArgument(Value* value);
  void printIR(ostream& stream) const override;

  Function* getFunction() const {
    return function;
  }
};

class IcmpInst : public Instruction {
 private:
  OpTag icmpType;

 public:
  IcmpInst(OpTag opType, Value* op1, Value* op2, string name);
  void printIR(ostream& stream) const override;

  const OpTag getOpTag() const {
    return icmpType;
  }
};

class FcmpInst : public Instruction {
 private:
  OpTag fcmpType;

 public:
  FcmpInst(OpTag opType, Value* op1, Value* op2, string name);
  void printIR(ostream& stream) const override;

  const OpTag getOpTag() const {
    return fcmpType;
  }
};

class FptosiInst : public Instruction {
 public:
  FptosiInst(Value* src, string name);
  void printIR(ostream& stream) const override;
};

class GetElemPtrInst : public Instruction {
 private:
  Type* ptrType;

 public:
  GetElemPtrInst(Value* ptr, Value* idx1, Value* idx2,
                 string name);
  GetElemPtrInst(Value* ptr, Value* idx1, string name);
  void printIR(ostream& stream) const override;
};

class JumpInst : public Instruction {
 public:
  JumpInst(BasicBlock* block);
  void printIR(ostream& stream) const override;
};

class LoadInst : public Instruction {
 public:
  LoadInst(Value* addr, string name);
  void printIR(ostream& stream) const override;
};

class PhiInst : public Instruction {
 public:
  PhiInst(string name);
  void printIR(ostream& stream) const override;
  void pushIncoming(Value* v, BasicBlock* bb);
};

class ReturnInst : public Instruction {
 public:
  ReturnInst(Value* retValue);
  ReturnInst();
  void printIR(ostream& stream) const override;
};

class SitofpInst : public Instruction {
 public:
  SitofpInst(Value* src, string name);
  void printIR(ostream& stream) const override;
};

class StoreInst : public Instruction {
 public:
  StoreInst(Value* value, Value* addr);
  void printIR(ostream& stream) const override;
};

// class UnaryOpInst : public Instruction {
//  private:
//   OpTag uOpType;

//  public:
//   UnaryOpInst(OpTag opType, Value* op1, string name);
//   void printIR(ostream& stream) const override;
// };

class ZextInst : public Instruction {
 public:
  ZextInst(Value* src, Type* dstType, string name);
  void printIR(ostream& stream) const override;
};

#endif