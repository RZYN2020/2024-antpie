#include "LoopInfo.hh"

LoopInfo* LoopInfo::getRootLoop() {
  LoopInfo* ptr = this;
  while (ptr->parentLoop) {
    ptr = ptr->parentLoop;
  }
  return ptr;
}

bool LoopInfo::containBlockInChildren(BasicBlock* block) {
  if (containBlock(block)) return true;

  for (LoopInfo* subloop : subLoops) {
    if (subloop->containBlockInChildren(block)) return true;
  }
  return false;
}

void LoopInfo::addBlock(BasicBlock* block) {
  if (liBase) {
    liBase->addBlockToLoop(block, this);
  } else {
    blocks.insert(block);
  }
}

void LoopInfo::addSubLoop(LoopInfo* subloop) {
  subLoops.push_back(subloop);
  subloop->setParentLoop(this);
}

void LoopInfo::dump() {
  std::cout << "Header: " << header->getName() << std::endl;
  std::cout << "Parent: " << (parentLoop ? parentLoop->getName() : "Null")
            << std::endl;
  std::cout << "SubLoop:";
  for (LoopInfo* subLoop : subLoops) {
    std::cout << subLoop->getName() << " ";
  }
  std::cout << std::endl;
  std::cout << "Blocks:";
  for (BasicBlock* block : blocks) {
    std::cout << block->getName() << " ";
  }
  std::cout << std::endl;
  std::cout << std::endl << std::endl;
}

LoopInfo* LoopInfoBase::getLoopOf(BasicBlock* block) {
  auto it = bbToLoop.find(block);
  if (it == bbToLoop.end()) {
    return nullptr;
  }
  return it->second;
}

void LoopInfoBase::addLoopInfo(LoopInfo* loopInfo) {
  loopInfos.push_back(loopInfo);
  for (BasicBlock* block : loopInfo->blocks) {
    bbToLoop[block] = loopInfo;
  }
}

void LoopInfoBase::addBlockToLoop(BasicBlock* block, LoopInfo* loopInfo) {
  loopInfo->blocks.insert(block);
  bbToLoop[block] = loopInfo;
}

void LoopInfoBase::calculateDepth() {
  std::function<void(LoopInfo*, uint32_t)> dfs = [&](LoopInfo* loopInfo,
                                                     uint32_t depth) {
    loopInfo->depth = depth;
    for (LoopInfo* subloop : loopInfo->subLoops) {
      dfs(subloop, depth + 1);
    }
  };

  for (LoopInfo* loopInfo : loopInfos) {
    if (!loopInfo->parentLoop) {
      dfs(loopInfo, 1);
    }
  }
}

uint32_t LoopInfoBase::getDepth(BasicBlock* block) {
  LoopInfo* loopInfo = getLoopOf(block);
  if (!loopInfo) return 0;
  return loopInfo->depth;
}

void LoopInfoBase::dump() {
  for (LoopInfo* loop : loopInfos) {
    loop->dump();
  }
}