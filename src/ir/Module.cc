#include "Module.hh"

#include "AliasAnalysis.hh"
#include "CommonSubexpElimination.hh"
#include "ConstantFolding.hh"
#include "DeadCodeElimination.hh"
#include "FunctionPropAnalysis.hh"
#include "GlobalCodeMotion.h"
#include "GlobalValueNumbering.hh"
#include "GlobalVariableLocalize.hh"
#include "Inlining.hh"
#include "LoadElimination.hh"
#include "LoopAnalysis.hh"
#include "LoopInvariantCodeMotion.hh"
#include "LoopSimplify.hh"
#include "LoopUnroll.hh"
#include "MemToReg.hh"
#include "MergeBlock.hh"
#include "StrengthReduction.hh"
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
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

BranchInst* Module::addBranchInst(Value* cond, BasicBlock* trueBlock,
                                  BasicBlock* falseBlock) {
  BranchInst* instr = new BranchInst(cond, trueBlock, falseBlock);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

CallInst* Module::addCallInst(Function* func, string name) {
  CallInst* instr = new CallInst(func, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

CallInst* Module::addCallInst(Function* func, vector<Value*>& params,
                              string name) {
  CallInst* instr = new CallInst(func, params, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

IcmpInst* Module::addIcmpInst(OpTag opType, Value* op1, Value* op2,
                              string name) {
  IcmpInst* instr = new IcmpInst(opType, op1, op2, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

FcmpInst* Module::addFcmpInst(OpTag opType, Value* op1, Value* op2,
                              string name) {
  FcmpInst* instr = new FcmpInst(opType, op1, op2, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

FptosiInst* Module::addFptosiInst(Value* src, string name) {
  FptosiInst* instr = new FptosiInst(src, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

GetElemPtrInst* Module::addGetElemPtrInst(Value* ptr, Value* idx1, Value* idx2,
                                          string name) {
  GetElemPtrInst* instr = new GetElemPtrInst(ptr, idx1, idx2, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

GetElemPtrInst* Module::addGetElemPtrInst(Value* ptr, Value* idx1,
                                          string name) {
  GetElemPtrInst* instr = new GetElemPtrInst(ptr, idx1, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

JumpInst* Module::addJumpInst(BasicBlock* block) {
  JumpInst* instr = new JumpInst(block);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

LoadInst* Module::addLoadInst(Value* addr, string name) {
  LoadInst* instr = new LoadInst(addr, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

PhiInst* Module::addPhiInst(string name) {
  PhiInst* instr = new PhiInst(name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

ReturnInst* Module::addReturnInst(Value* retValue) {
  ReturnInst* instr = new ReturnInst(retValue);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

ReturnInst* Module::addReturnInst() {
  ReturnInst* instr = new ReturnInst();
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

SitofpInst* Module::addSitofpInst(Value* src, string name) {
  SitofpInst* instr = new SitofpInst(src, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

StoreInst* Module::addStoreInst(Value* value, Value* addr) {
  StoreInst* instr = new StoreInst(value, addr);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
  return instr;
}

ZextInst* Module::addZextInst(Value* src, Type* dstType, string name) {
  ZextInst* instr = new ZextInst(src, dstType, name);
  if (!currBasicBlock->pushInstr(instr)) {
    BasicBlock* newBlock =
        addBasicBlock(currBasicBlock->getParent(), currBasicBlock->getName());
    newBlock->pushInstr(instr);
  }
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
  optimizations.pushBack(new DeadCodeElimination());
  optimizations.pushBack(new FunctionPropAnalysis());

  optimizations.pushBack(new MemToReg());
  optimizations.pushBack(new Inlining());
  optimizations.pushBack(new FunctionPropAnalysis());
  optimizations.pushBack(new GlobalVariableLocalize());
  optimizations.pushBack(new ConstantFolding());
  optimizations.pushBack(new StrengthReduction());
  optimizations.pushBack(new DeadCodeElimination());
  optimizations.pushBack(new MergeBlock());
  optimizations.pushBack(new CommonSubexpElimination());
  optimizations.pushBack(new LoopAnalysis());
  optimizations.pushBack(new LoopSimplify());
  optimizations.pushBack(new AliasAnalysis());
  optimizations.pushBack(new LoopInvariantCodeMotion());
  optimizations.pushBack(new LoopUnroll());
  optimizations.pushBack(new DeadCodeElimination());
  // GVM and GCM need DCE
  optimizations.pushBack(new GlobalValueNumbering());
  optimizations.pushBack(new GlobalCodeMotion());
  optimizations.pushBack(new MergeBlock());
  optimizations.pushBack(new AliasAnalysis());
  optimizations.pushBack(new LoadElimination());
  optimizations.pushBack(new LoopAnalysis());
  optimizations.pushBack(new LoopSimplify());
  optimizations.pushBack(new LoopInvariantCodeMotion());
  optimizations.pushBack(new TailRecursionElimination());
  optimizations.pushBack(new MergeBlock());
  optimizations.pushBack(new DeadCodeElimination());

  // run all pass
  for (auto& pass : optimizations) {
    pass->runOnModule(this);
    // if (dynamic_cast<LoopUnroll*>(pass)) printIR(std::cout);
  }

  for (Function* func : functions) {
    func->buildCFG();
    func->buildDT();
  }
}