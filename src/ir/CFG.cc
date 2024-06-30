#include "CFG.hh"

void CFG::addEdge(BasicBlock* src, BasicBlock* dest) {
  blkPredMap[dest]->pushBack(src);
  blkSuccMap[src]->pushBack(dest);
}

// build CFG
CFG::CFG(Function* func) {
  entry = func->getEntry();
  exit = func->getExit();

  auto bbList = func->getBasicBlocks();
  for (auto bb = bbList->begin(); bb != bbList->end(); ++bb) {
    blkPredMap[*bb] = new LinkedList<BasicBlock*>();
    blkSuccMap[*bb] = new LinkedList<BasicBlock*>();
    blocks.pushBack(*bb);
  }

  for (auto bb = bbList->begin(); bb != bbList->end(); ++bb) {
    if (*bb == exit) {
      continue;
    }
    Instruction* tailInstr = (*bb)->getTailInstr();
    if (!tailInstr) {
      addEdge(*bb, *(bb + 1));
      continue;
    }
    ValueTag tailType = tailInstr->getValueTag();
    if (tailType == VT_BR) {
      BranchInst* brInstr = static_cast<BranchInst*>(tailInstr);
      BasicBlock* trueBB = static_cast<BasicBlock*>(brInstr->getRValue(1));
      BasicBlock* falseBB = static_cast<BasicBlock*>(brInstr->getRValue(2));
      addEdge(*bb, trueBB);
      addEdge(*bb, falseBB);
    } else if (tailType == VT_JUMP) {
      JumpInst* jumpInst = static_cast<JumpInst*>(tailInstr);
      BasicBlock* destBB = static_cast<BasicBlock*>(jumpInst->getRValue(0));
      addEdge(*bb, destBB);
    } else if (tailType == VT_RET) {
      addEdge(*bb, exit);
    } else {
      addEdge(*bb, *(bb + 1));
    }
  }
  // debug();
}

void CFG::debug() {
  for (auto bb = blocks.begin(); bb != blocks.end(); ++bb) {
    std::cout << (*bb)->getName() << ":" << std::endl;
    BBListPtr preds = getPredOf(*bb);
    BBListPtr succs = getSuccOf(*bb);
    for (auto pbb = preds->begin(); pbb != preds->end(); ++pbb) {
      std::cout << (*pbb)->getName() << " ";
    }
    std::cout << std::endl;
    for (auto sbb = succs->begin(); sbb != succs->end(); ++sbb) {
      std::cout << (*sbb)->getName() << " ";
    }
    std::cout << std::endl;
  }
}