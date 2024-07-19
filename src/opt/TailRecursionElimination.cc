#include "TailRecursionElimination.hh"

// Check whether function have tail recursion call,
// and collect tail recursion call instructions
bool TailRecursionElimination::findTailRecursionBlocks(
    Function* function, vector<CallInst*>& tailRecurCalls) {
  for (BasicBlock* block : *function->getBasicBlocks()) {
    if (!block->getTailInstr() || !block->getTailInstr()->isa(VT_RET)) {
      continue;
    }
    uint32_t size = block->getInstrSize();
    if (size < 2) return false;
    Instruction* preInstr = block->getInstructionAt(size - 2);
    if (CallInst* callInst = dynamic_cast<CallInst*>(preInstr)) {
      if (callInst->getFunction() == function) {
        tailRecurCalls.push_back(callInst);
      }
    }
  }
  return !tailRecurCalls.empty();
}

bool TailRecursionElimination::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  for (Function* function : *module->getFunctions()) {
    changed |= runOnFunction(function);
  }
  return changed;
}
bool TailRecursionElimination::runOnFunction(Function* func) {
  vector<CallInst*> tailRecurCalls;
  if (!findTailRecursionBlocks(func, tailRecurCalls)) return false;

  // Create a new phi block for argument
  BasicBlock* phiBlock = new BasicBlock(LabelManager::getLabel("phi_bb"));
  func->pushBasicBlock(phiBlock);
  FuncType* funcType = (FuncType*)func->getType();
  int argSize = funcType->getArgSize();
  BasicBlock* entry = func->getEntry();

  // Build a new phi instruction for every argument
  for (int i = 0; i < argSize; i++) {
    Argument* arg = funcType->getArgument(i);
    PhiInst* phiInst = new PhiInst(arg->getName());
    for (CallInst* tailCall : tailRecurCalls) {
      phiInst->pushIncoming(tailCall->getRValue(i), tailCall->getParent());
    }
    phiBlock->pushInstr(phiInst);
    arg->replaceAllUsesWith(phiInst);
    phiInst->pushIncoming(arg, entry);
  }

  // Move instruction except alloca from entry to phi
  for (auto it = entry->getInstructions()->begin();
       it != entry->getInstructions()->end();) {
    Instruction* instr = *it;
    ++it;
    if (!instr->isa(VT_ALLOCA)) {
      instr->eraseFromParent();
      phiBlock->pushInstr(instr);
    }
  }

  // Modify CFG
  if (func->getCFG()) {
    CFG* cfg = func->getCFG();
    cfg->addNode(phiBlock);
    for (BasicBlock* entrySucc : *cfg->getSuccOf(entry)) {
      cfg->eraseEdge(entry, entrySucc);
      cfg->addEdge(phiBlock, entrySucc);
    }
    cfg->addEdge(entry, phiBlock);
    for (CallInst* tailCall : tailRecurCalls) {
      BasicBlock* tailBlock = tailCall->getParent();
      cfg->addEdge(tailBlock, phiBlock);
    }
  }
  // Modify control flow:
  // entry -> phinode
  // phinode -> succ(entry)
  // tail block -> phinode
  entry->pushInstr(new JumpInst(phiBlock));
  for (CallInst* callInst : tailRecurCalls) {
    BasicBlock* tailBlock = callInst->getParent();
    Instruction* tailInst = tailBlock->getTailInstr();
    tailInst->eraseFromParent();
    tailInst->deleteUseList();
    callInst->eraseFromParent();
    callInst->deleteUseList();
    tailBlock->pushInstr(new JumpInst(phiBlock));
  }
  func->resetDT();
  return true;
}