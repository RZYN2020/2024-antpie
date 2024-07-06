#include "Module.hh"

#include "MemToReg.hh"

using ANTPIE::Module;

Module::Module() {}

Module::~Module() {
  for (const auto& gv : globalVariables) {
    delete gv;
  }
  for (const auto& func : functions) {
    delete func;
  }
}
void Module::pushFunction(Function* function) { functions.pushBack(function); }

void Module::pushGlobalVariable(GlobalVariable* globalVariable) {
  globalVariables.pushBack(globalVariable);
}

Function* Module::addFunction(FuncType* funcType, string name) {
  Function* func = new Function(funcType, name);
  functions.pushBack(func);
  return func;
}

BasicBlock* Module::addBasicBlock(Function* function, string name) {
  BasicBlock* bb = new BasicBlock(name);
  function->pushBasicBlock(bb);
  return bb;
}

void Module::printIR(ostream& stream) const {
  for (const auto& gv : globalVariables) {
    gv->printIR(stream);
    stream << endl;
  }
  stream << endl;

  for (const auto f : functions) {
    f->printIR(stream);
    stream << endl;
  }
}

AllocaInst* Module::addAllocaInst(Type* type, string name) {
  AllocaInst* instr = new AllocaInst(type, name);
  functions.back()->getEntry()->pushInstrAtHead(instr);
  return instr;
}

BinaryOpInst* Module::addBinaryOpInst(OpTag opType, Value* op1, Value* op2,
                                      string name) {
  BinaryOpInst* instr = new BinaryOpInst(opType, op1, op2, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

BranchInst* Module::addBranchInst(Value* cond, BasicBlock* trueBlock,
                                  BasicBlock* falseBlock) {
  BranchInst* instr = new BranchInst(cond, trueBlock, falseBlock);
  currBasicBlock->pushInstr(instr);
  return instr;
}

CallInst* Module::addCallInst(Function* func, string name) {
  CallInst* instr = new CallInst(func, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

CallInst* Module::addCallInst(Function* func, vector<Value*>& params,
                              string name) {
  CallInst* instr = new CallInst(func, params, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

IcmpInst* Module::addIcmpInst(OpTag opType, Value* op1, Value* op2,
                              string name) {
  IcmpInst* instr = new IcmpInst(opType, op1, op2, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

FcmpInst* Module::addFcmpInst(OpTag opType, Value* op1, Value* op2,
                              string name) {
  FcmpInst* instr = new FcmpInst(opType, op1, op2, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

FptosiInst* Module::addFptosiInst(Value* src, string name) {
  FptosiInst* instr = new FptosiInst(src, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

GetElemPtrInst* Module::addGetElemPtrInst(Value* ptr, Value* idx1, Value* idx2,
                                          string name) {
  GetElemPtrInst* instr = new GetElemPtrInst(ptr, idx1, idx2, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

GetElemPtrInst* Module::addGetElemPtrInst(Value* ptr, Value* idx1,
                                          string name) {
  GetElemPtrInst* instr = new GetElemPtrInst(ptr, idx1, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

JumpInst* Module::addJumpInst(BasicBlock* block) {
  JumpInst* instr = new JumpInst(block);
  currBasicBlock->pushInstr(instr);
  return instr;
}

LoadInst* Module::addLoadInst(Value* addr, string name) {
  LoadInst* instr = new LoadInst(addr, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

PhiInst* Module::addPhiInst(string name) {
  PhiInst* instr = new PhiInst(name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

ReturnInst* Module::addReturnInst(Value* retValue) {
  ReturnInst* instr = new ReturnInst(retValue);
  currBasicBlock->pushInstr(instr);
  return instr;
}

ReturnInst* Module::addReturnInst() {
  ReturnInst* instr = new ReturnInst();
  currBasicBlock->pushInstr(instr);
  return instr;
}

SitofpInst* Module::addSitofpInst(Value* src, string name) {
  SitofpInst* instr = new SitofpInst(src, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

StoreInst* Module::addStoreInst(Value* value, Value* addr) {
  StoreInst* instr = new StoreInst(value, addr);
  currBasicBlock->pushInstr(instr);
  return instr;
}

ZextInst* Module::addZextInst(Value* src, Type* dstType, string name) {
  ZextInst* instr = new ZextInst(src, dstType, name);
  currBasicBlock->pushInstr(instr);
  return instr;
}

GlobalVariable* Module::addGlobalVariable(Type* type, string name) {
  GlobalVariable* gv = new GlobalVariable(type, name);
  globalVariables.pushBack(gv);
  return gv;
}

GlobalVariable* Module::addGlobalVariable(Type* type, Constant* init,
                                          string name) {
  GlobalVariable* gv = new GlobalVariable(type, init, name);
  globalVariables.pushBack(gv);
  return gv;
}

void Module::buildCFG() {
  for (const auto& func : functions) {
    func->buildCFG();
  }
}

void Module::irOptimize() {
  LinkedList<Optimization*> optimizations;

  // Add mem2reg Pass
  optimizations.pushBack(new MemToReg());

  // run all pass
  for (auto& pass : optimizations) {
    pass->runOnModule(this);
  }
}