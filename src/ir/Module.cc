#include "Module.hh"

#include "AliasAnalysis.hh"
#include "CommonSubexpElimination.hh"
#include "DeadCodeElimination.hh"
#include "FunctionPropAnalysis.hh"
#include "GlobalCodeMotion.h"
#include "GlobalValueNumbering.hh"
#include "Inlining.hh"
#include "LoopAnalysis.hh"
#include "LoopInvariantCodeMotion.hh"
#include "LoopSimplify.hh"
#include "LoopUnroll.hh"
#include "MemToReg.hh"
#include "MergeBlock.hh"
#include "TailRecursionElimination.hh"

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

void Module::pushExternFunction(Function* function) {
  externFunctions.insert(function);
}

Function* Module::addFunction(FuncType* funcType, string name) {
  Function* func = new Function(funcType, name);
  functions.pushBack(func);
  func->setParent(this);
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

  for (const auto ef : externFunctions) {
    ef->printIR(stream);
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
  // Add dead code elimination
  optimizations.pushBack(new DeadCodeElimination());
  // Add mem2reg Pass
  optimizations.pushBack(new MemToReg());
  // // Add function analysis pass
  optimizations.pushBack(new FunctionPropAnalysis());
  // // Add function inlining pass
  optimizations.pushBack(new Inlining());

  // // Add mergeBlock pass
  optimizations.pushBack(new MergeBlock());
  // // Add earlyCSE pass
  optimizations.pushBack(new CommonSubexpElimination());
  // Add loop Analysis
  optimizations.pushBack(new LoopAnalysis());
  // // Add loop simplify pass
  optimizations.pushBack(new LoopSimplify());
  // // Add alias analysis pass
  optimizations.pushBack(new AliasAnalysis());
  // // Add LICM pass
  optimizations.pushBack(new LoopInvariantCodeMotion());
  // Add Loop Unroll pass
  optimizations.pushBack(new LoopUnroll());
  optimizations.pushBack(new DeadCodeElimination());
  // // GVM and GCM need DCE
  // Add GVM pass
  optimizations.pushBack(new GlobalValueNumbering());
  // Add GCM pass
  optimizations.pushBack(new GlobalCodeMotion());

  // Add tail recursion elimination
  optimizations.pushBack(new TailRecursionElimination());
  optimizations.pushBack(new MergeBlock());

  // run all pass
  for (auto& pass : optimizations) {
    // if (dynamic_cast<DeadCodeElimination*>(pass)) {
    //   printIR(std::cout);
    // }
    pass->runOnModule(this);
    // if (dynamic_cast<LoopUnroll*>(pass)) {
    //   printIR(std::cout);
    // }
  }
}