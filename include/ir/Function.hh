/**
 * Function ir
 * Create by Zhang Junbin at 2024/6/2
 */

#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "BasicBlock.hh"
#include "DomTree.hh"
#include "Type.hh"
#include "Value.hh"

class CFG;
namespace ANTPIE {
class Module;
}

class Function : public GlobalValue {
 private:
  LinkedList<BasicBlock*> basicBlocks;
  CFG* cfg = 0;
  DomTree* dt = 0;
  BasicBlock *entry, *exit;
  ANTPIE::Module* module;

  struct FunctionProperties {
    bool isRecursive : 1;
    bool hasSideEffects : 1;
    bool pureFunction : 1;
    size_t lineSize;
    unordered_set<Function*> callerFunctions;
    unordered_set<Function*> calledFunctions;

    FunctionProperties()
        : isRecursive(false), hasSideEffects(false), pureFunction(true) {}

    void addCalled(Function* func) { calledFunctions.insert(func); }

    void addCaller(Function* func) { callerFunctions.insert(func); }

  } props;

 public:
  Function(FuncType* fType, string name);
  ~Function();
  void pushBasicBlock(BasicBlock* bb);
  void pushBasicBlockAtHead(BasicBlock* bb);
  void printIR(ostream& stream) const override;
  void setParent(ANTPIE::Module* module_) { module = module_; }
  ANTPIE::Module* getParent() const { return module; }

  LinkedList<BasicBlock*>* getBasicBlocks() { return &basicBlocks; }

  BasicBlock* getEntry() const { return entry; }
  BasicBlock* getExit() const { return exit; }

  CFG* buildCFG();
  CFG* getCFG() { return cfg; }
  void resetCFG() { cfg = 0; }

  DomTree* buildDT();
  DomTree* getDT() { return dt; }
  void resetDT() { dt = 0; }

  bool isRecursive() { return props.isRecursive; }
  bool hasSideEffects() { return props.hasSideEffects; }
  bool isPureFunction() { return props.pureFunction; }
  size_t getLineSize() { return props.lineSize; }

  void setRecursive(bool isRecure) { props.isRecursive = isRecure; }
  void setSideEffects(bool sideEffect) { props.hasSideEffects = sideEffect; }
  void setPureFunction(bool pureFunction) { props.pureFunction = pureFunction; }
  void setSize(size_t size) { props.lineSize = size; }

  unordered_set<Function*>& getCallers() { return props.callerFunctions; }
  unordered_set<Function*>& getCallees() { return props.calledFunctions; }
  void clearCall() {
    props.callerFunctions.clear();
    props.calledFunctions.clear();
  }

  static void addCall(Function* caller, Function* callee) {
    caller->props.addCalled(callee);
    callee->props.addCaller(caller);
  }
};

#endif