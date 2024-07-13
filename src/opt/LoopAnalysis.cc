#include "LoopAnalysis.hh"

bool LoopAnalysis::runOnModule(ANTPIE::Module* module) {
  for (Function* func : *module->getFunctions()) {
    runOnFunction(func);
  }
  return true;
}

bool LoopAnalysis::runOnFunction(Function* func) {
  CFG* cfg = func->getCFG();
  if (!cfg) cfg = func->buildCFG();
  DomTree* dt = func->getDT();
  if (!dt) dt = func->buildDT();

  LoopInfoBase* loopInfoBase = new LoopInfoBase();

  BBListPtr postOrderDT = dt->postOrder();

  for (BasicBlock* header : *postOrderDT) {
    vector<BasicBlock*> latches;

    for (BasicBlock* latch : *cfg->getPredOf(header)) {
      if (dt->dominates(header, latch)) {
        latches.push_back(latch);
      }
    }

    if (!latches.empty()) {
      LoopInfo* loopInfo = new LoopInfo(header);
      discoverAndMapSubloop(loopInfo, latches, loopInfoBase, dt, cfg);
      loopInfoBase->addLoopInfo(loopInfo);
    }
  }
  func->setLoopInfoBase(loopInfoBase);

#ifdef DEBUG_MODE
  loopInfoBase->dump();
#endif

  return true;
}

void LoopAnalysis::discoverAndMapSubloop(LoopInfo* loopInfo,
                                         vector<BasicBlock*>& latches,
                                         LoopInfoBase* li, DomTree* dt,
                                         CFG* cfg) {
  LinkedList<BasicBlock*> workList;
  for (BasicBlock* latch : latches) {
    workList.pushBack(latch);
  }

  while (!workList.isEmpty()) {
    BasicBlock* pred = workList.popFront();

    LoopInfo* subloop = li->getLoopOf(pred);
    if (!subloop) {
      loopInfo->addBlock(pred);
      if (pred == loopInfo->header) {
        continue;
      }
      for (BasicBlock* ppred : *cfg->getPredOf(pred)) {
        workList.pushBack(ppred);
      }
    } else {
      subloop = subloop->getRootLoop();

      if (subloop == loopInfo) {
        continue;
      }

      loopInfo->addSubLoop(subloop);

      BasicBlock* subHeader = subloop->header;
      for (BasicBlock* shPred : *cfg->getPredOf(subHeader)) {
        if (!subloop->containBlock(shPred)) {
          workList.pushBack(shPred);
        }
      }
    }
  }
}
