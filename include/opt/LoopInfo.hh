#ifndef _LOOPINFO_H_
#define _LOOPINFO_H_

#include <unordered_set>
#include <vector>

#include "BasicBlock.hh"
using std::unordered_set;
using std::vector;

struct LoopInfo {
  LoopInfo* parentLoop;
  vector<LoopInfo*> subLoops;
  unordered_set<BasicBlock*> blocks;

  BasicBlock* preHeader;
  BasicBlock* header;
  BasicBlock* latch;
  LoopInfo(BasicBlock* header_) : header(header_), parentLoop(nullptr) {}

  // Get the most outside loop
  LoopInfo* getRootLoop() {
    LoopInfo* ptr = this;
    while (ptr->parentLoop) {
      ptr = ptr->parentLoop;
    }
    return ptr;
  }

  bool containBlock(BasicBlock* block) { return blocks.count(block); }

  bool containBlockInChildren(BasicBlock* block) {
    if (containBlock(block)) return true;

    for (LoopInfo* subloop : subLoops) {
      if (subloop->containBlockInChildren(block)) return true;
    }
    return false;
  }
  
  void addBlock(BasicBlock* block) { blocks.insert(block); }
  void setParentLoop(LoopInfo* parent) { parentLoop = parent; }

  void addSubLoop(LoopInfo* subloop) {
    subLoops.push_back(subloop);
    subloop->setParentLoop(this);
  }

  string getName() { return header->getName() + "Loop"; }

  void dump() {
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
};

struct LoopInfoBase {
  vector<LoopInfo*> loopInfos;

  LoopInfo* getLoopOf(BasicBlock* block) {
    for (LoopInfo* loopInfo : loopInfos) {
      if (loopInfo->blocks.count(block)) {
        return loopInfo;
      }
    }
    return nullptr;
  }

  void addLoopInfo(LoopInfo* loopInfo) { loopInfos.push_back(loopInfo); }

  void dump() {
    for (LoopInfo* loop : loopInfos) {
      loop->dump();
    }
  }
};

#endif
