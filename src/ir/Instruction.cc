#include "../../include/ir/Instruction.hh"

#include "../../include/ir/Type.hh"

Instruction::Instruction(ValueTag vTag) : Value(nullptr, vTag) {
  useList = make_unique<vector<Use*>>();
  valueList = make_unique<vector<Value*>>();
}

Instruction::Instruction(string name, ValueTag vTag)
    : Value(nullptr, name, vTag) {
  useList = make_unique<vector<Use*>>();
  valueList = make_unique<vector<Value*>>();
}

Instruction::Instruction(Type* t, string name, ValueTag vTag)
    : Value(t, name, vTag) {
  useList = make_unique<vector<Use*>>();
  valueList = make_unique<vector<Value*>>();
}

void Instruction::pushValue(Value* v) { valueList->push_back(v); };

AllocaInst::AllocaInst(Type* type, string name)
    : Instruction(name, VT_ALLOCA), elemType(type) {}

BinaryOpInst::BinaryOpInst(OpTag opType, Value* op1, Value* op2, string name)
    : Instruction(name, VT_BOP), bOpType(opType) {
  pushValue(op1);
  pushValue(op2);
};

BranchInst::BranchInst(Value* cond, BasicBlock* trueBlock,
                       BasicBlock* falseBlock)
    : Instruction(VT_BR) {
  pushValue(cond);
  pushValue((Value*)trueBlock);
  pushValue((Value*)falseBlock);
}

CallInst::CallInst(Function* func, vector<Value*>& params, string name)
    : Instruction(name, VT_CALL), function(func) {
  for (Value* param : params) {
    pushValue(param);
  }
}

IcmpInst::IcmpInst(OpTag opType, Value* op1, Value* op2, string name)
    : Instruction(name, VT_ICMP), icmpType(opType) {
  pushValue(op1);
  pushValue(op2);
}

FcmpInst::FcmpInst(OpTag opType, Value* op1, Value* op2, string name)
    : Instruction(name, VT_FCMP), fcmpType(opType) {
  pushValue(op1);
  pushValue(op2);
}

FptosiInst::FptosiInst(Value* src, string name) : Instruction(name, VT_FPTOSI) {
  pushValue(src);
}

GetElemPtrInst::GetElemPtrInst(Type* pType, Value* ptr, Value* idx1,
                               Value* idx2, string name)
    : Instruction(name, VT_GEP), ptrType(pType) {
  pushValue(ptr);
  pushValue(idx1);
  pushValue(idx2);
}

JumpInst::JumpInst(BasicBlock* block) : Instruction(VT_JUMP) {
  pushValue((Value*)block);
}

LoadInst::LoadInst(Value* addr, string name) : Instruction(name, VT_LOAD) {
  pushValue(addr);
}

PhiInst::PhiInst(string name) : Instruction(name, VT_PHI) {}

ReturnInst::ReturnInst(Value* retValue) : Instruction(VT_RET) {
  pushValue(retValue);
}

ReturnInst::ReturnInst() : Instruction(VT_RET) {}

SitofpInst::SitofpInst(Value* src, string name) : Instruction(name, VT_SITOFP) {
  pushValue(src);
}

StoreInst::StoreInst(Value* value, Value* addr) : Instruction(VT_STORE) {
  pushValue(value);
  pushValue(addr);
}

ZextInst::ZextInst(Value* src, Type* dstType, string name)
    : Instruction(dstType, name, VT_ZEXT) {
  pushValue(src);
}

void AllocaInst::printIR(ostream& stream) const {
  stream << "%" << getName() << " = alloca " << elemType->toString();
}

string Instruction::getOpName(OpTag op) const {
  static string opNames[25] = {
      "add",  "fadd", "sub", "fsub", "mul", "fmul", "sdiv", "fdiv", "srem",
      "frem", "and",  "or",  "xor",  "eq",  "ne",   "sle",  "slt",  "sge",
      "sgt",  "oeq",  "one", "ogt",  "oge", "ole",  "olt"};
  return opNames[op];
}

void BinaryOpInst::printIR(ostream& stream) const {}

void BranchInst::printIR(ostream& stream) const {}

void CallInst::printIR(ostream& stream) const {}

void IcmpInst::printIR(ostream& stream) const {}

void FcmpInst::printIR(ostream& stream) const {}

void FptosiInst::printIR(ostream& stream) const {}

void GetElemPtrInst::printIR(ostream& stream) const {}

void JumpInst::printIR(ostream& stream) const {}

void LoadInst::printIR(ostream& stream) const {}

void PhiInst::printIR(ostream& stream) const {}

void ReturnInst::printIR(ostream& stream) const {}

void SitofpInst::printIR(ostream& stream) const {}

void StoreInst::printIR(ostream& stream) const {}

void ZextInst::printIR(ostream& stream) const {}
