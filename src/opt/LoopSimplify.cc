#include "LoopSimplify.hh"

bool LoopSimplify::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  for (Function* function : *module->getFunctions()) {
    changed |= runOnFunction(function);
  }
  return changed;
}

bool LoopSimplify::runOnFunction(Function* func) {
  bool changed = false;
  LoopInfoBase* loopInfoBase = func->getLoopInfoBase();
  CFG* cfg = func->getCFG();
  if (!cfg) cfg = func->buildCFG();

  assert(loopInfoBase);
  for (LoopInfo* loopInfo : loopInfoBase->loopInfos) {
    changed |= simplyOneLoop(loopInfo, cfg);
  }

  if (changed) {
    func->resetDT();
  }
  return changed;
}

bool LoopSimplify::simplyOneLoop(LoopInfo* loopInfo, CFG* cfg) {
  bool changed = false;

  // 1. remove dead block (Is it need?)

  // 2. build preHeader
  BasicBlock* header = loopInfo->header;
  vector<BasicBlock*> outsideBlocks;
  for (BasicBlock* pred : *cfg->getPredOf(header)) {
    if (!loopInfo->containBlock(pred)) {
      outsideBlocks.push_back(pred);
    }
  }
  if (!loopInfo->preHeader) {
    loopInfo->preHeader = header->splitBlockPredecessors(outsideBlocks);
    changed = true;
    if (loopInfo->parentLoop) {
      loopInfo->parentLoop->addBlock(loopInfo->preHeader);
    }
  }


  return changed;
}
