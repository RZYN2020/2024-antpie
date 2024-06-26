#include "Instruction.hh"

#include <cassert>

#include "ir/Function.hh"
#include "ir/Type.hh"

Instruction::Instruction(ValueTag vTag) : Value(Type::getVoidType(), vTag) {
  useList = make_unique<vector<Use*>>();
  valueList = make_unique<vector<Value*>>();
}

Instruction::Instruction(string name, ValueTag vTag)
    : Value(Type::getVoidType(), name, vTag) {
  useList = make_unique<vector<Use*>>();
  valueList = make_unique<vector<Value*>>();
}

Instruction::Instruction(Type* t, string name, ValueTag vTag)
    : Value(t, name, vTag) {
  useList = make_unique<vector<Use*>>();
  valueList = make_unique<vector<Value*>>();
}

void Instruction::pushValue(Value* v) { valueList->push_back(v); };

Value* Instruction::getRValue(int idx) const {
  assert(idx < valueList->size());
  return valueList->at(idx);
}

AllocaInst::AllocaInst(Type* type, string name)
    : Instruction(Type::getPointerType(type), name, VT_ALLOCA), elemType(type) {}

BinaryOpInst::BinaryOpInst(OpTag opType, Value* op1, Value* op2, string name)
    : Instruction(op1->getType(), name, VT_BOP), bOpType(opType) {
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
  setType(((FuncType*)func->getType())->getRetType());
  for (Value* param : params) {
    pushValue(param);
  }
}

CallInst::CallInst(Function* func, string name)
    : Instruction(name, VT_CALL), function(func) {
  setType(((FuncType*)func->getType())->getRetType());
}

void CallInst::pushArgument(Value* value) { pushValue(value); }

IcmpInst::IcmpInst(OpTag opType, Value* op1, Value* op2, string name)
    : Instruction(Type::getInt1Type(), name, VT_ICMP), icmpType(opType) {
  pushValue(op1);
  pushValue(op2);
}

FcmpInst::FcmpInst(OpTag opType, Value* op1, Value* op2, string name)
    : Instruction(Type::getInt1Type(), name, VT_FCMP), fcmpType(opType) {
  pushValue(op1);
  pushValue(op2);
}

FptosiInst::FptosiInst(Value* src, string name)
    : Instruction(Type::getInt32Type(), name, VT_FPTOSI) {
  pushValue(src);
}

GetElemPtrInst::GetElemPtrInst(Value* ptr, Value* idx1, Value* idx2,
                               string name)
    : Instruction(name, VT_GEP), ptrType(ptr->getType()) {
  setType(Type::getPointerType(
      ((ArrayType*)((PointerType*)ptrType)->getElemType())->getElemType()));
  pushValue(ptr);
  pushValue(idx1);
  pushValue(idx2);
}

GetElemPtrInst::GetElemPtrInst(Value* ptr, Value* idx1, string name)
    : Instruction(ptr->getType(), name, VT_GEP), ptrType(ptr->getType()) {
  pushValue(ptr);
  pushValue(idx1);
}

JumpInst::JumpInst(BasicBlock* block) : Instruction(VT_JUMP) {
  pushValue((Value*)block);
}

LoadInst::LoadInst(Value* addr, string name) : Instruction(name, VT_LOAD) {
  setType(((PointerType*)addr->getType())->getElemType());
  pushValue(addr);
}

PhiInst::PhiInst(string name) : Instruction(name, VT_PHI) {}

void PhiInst::pushIncoming(Value* v, BasicBlock* bb) {
  pushValue(v);
  pushValue((Value*)bb);
  setType(v->getType());
}

ReturnInst::ReturnInst(Value* retValue) : Instruction(VT_RET) {
  pushValue(retValue);
}

ReturnInst::ReturnInst() : Instruction(VT_RET) {}

SitofpInst::SitofpInst(Value* src, string name)
    : Instruction(Type::getFloatType(), name, VT_SITOFP) {
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

void BinaryOpInst::printIR(ostream& stream) const {
  stream << toString() << " = " << getOpName(bOpType) << " "
         << getType()->toString() << " " << getRValue(0)->toString() << ", "
         << getRValue(1)->toString();
}

// br i1 %cmp, label %if.then, label %if.else
void BranchInst::printIR(ostream& stream) const {
  Value* cond = getRValue(0);
  stream << "br " << cond->getType()->toString() << " " << cond->toString()
         << ", label " << getRValue(1)->toString() << ", label "
         << getRValue(2)->toString();
}

// %call = call i32 @foo(i32 noundef 1, i32 noundef 2)
void CallInst::printIR(ostream& stream) const {
  Type* retType = ((FuncType*)function->getType())->getRetType();
  if (retType->getTypeTag() != TT_VOID) {
    stream << toString() << " = ";
  }
  stream << "call " << retType->toString() << " @" << function->getName()
         << "(";
  int vSize = getRValueSize();
  for (int i = 0; i < vSize; i++) {
    if (i != 0) {
      stream << ", ";
    }
    Value* arg = getRValue(i);
    stream << arg->getType()->toString() << " " << arg->toString();
  }
  stream << ")";
}

void IcmpInst::printIR(ostream& stream) const {
  stream << toString() << " = icmp " << getOpName(icmpType) << " i32 "
         << getRValue(0)->toString() << ", " << getRValue(1)->toString();
}

void FcmpInst::printIR(ostream& stream) const {
  stream << toString() << " = fcmp " << getOpName(fcmpType) << " float "
         << getRValue(0)->toString() << ", " << getRValue(1)->toString();
}
// %conv1 = fptosi float %add to i32
void FptosiInst::printIR(ostream& stream) const {
  Value* src = getRValue(0);
  stream << toString() << " = fptosi " << src->getType()->toString() << " "
         << src->toString() << " to i32";
}

// %arrayidx = getelementptr inbounds [5 x i32], ptr %arr, i32 0, i32 1
void GetElemPtrInst::printIR(ostream& stream) const {
  stream << toString() << " = getelementptr inbounds "
         << ((PointerType*)ptrType)->getElemType()->toString();
  int argSize = getRValueSize();
  for (int i = 0; i < argSize; i++) {
    Value* arg = getRValue(i);
    stream << ", " << arg->getType()->toString() << " " << arg->toString();
  }
}

void JumpInst::printIR(ostream& stream) const {
  stream << "br label " << getRValue(0)->toString();
}

// %0 = load i32, ptr %a.addr
void LoadInst::printIR(ostream& stream) const {
  stream << toString() << " = load " << getType()->toString() << ", "
         << getRValue(0)->getType()->toString() << " "
         << getRValue(0)->toString();
}

// %y = phi i32 [ %y.0, %if.then ], [ %neg, %if.else ]
void PhiInst::printIR(ostream& stream) const {
  int icSize = getRValueSize() / 2;
  assert(icSize);
  stream << toString() << " = phi " << getType()->toString();
  for (int i = 0; i < icSize; i++) {
    if (i != 0) {
      stream << ", ";
    }
    stream << "[ " << getRValue(i * 2)->toString() << ", "
           << getRValue(i * 2 + 1)->toString() << " ]";
  }
}

// ret i32 %conv1
void ReturnInst::printIR(ostream& stream) const {
  stream << "ret ";
  if (getRValueSize() == 0) {
    stream << "void";
  } else {
    Value* rValue = getRValue(0);
    stream << rValue->getType()->toString() << " " << rValue->toString();
  }
}

// %conv = sitofp i32 %1 to float
void SitofpInst::printIR(ostream& stream) const {
  Value* src = getRValue(0);
  stream << toString() << " = sitofp " << src->getType()->toString() << " "
         << src->toString() << " to float";
}

// store i32 %a, ptr %a.addr
void StoreInst::printIR(ostream& stream) const {
  Value* src = getRValue(0);
  Value* dst = getRValue(1);
  stream << "store " << src->getType()->toString() << " " << src->toString()
         << ", " << dst->getType()->toString() << " " << dst->toString();
}

// %lnot.ext = zext i1 %lnot to i32
void ZextInst::printIR(ostream& stream) const {
  Value* src = getRValue(0);
  stream << toString() << " = zext " << src->getType()->toString() << " "
         << src->toString() << " to " << getType()->toString();
}
