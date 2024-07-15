#ifndef _LOOPINFO_H_
#define _LOOPINFO_H_

#include <functional>
#include <unordered_set>
#include <vector>

#include "BasicBlock.hh"
using std::unordered_set;
using std::vector;

struct LoopInfoBase;

struct LoopInfo {
  LoopInfoBase* liBase;
  LoopInfo* parentLoop;
  vector<LoopInfo*> subLoops;
  unordered_set<BasicBlock*> blocks;

  BasicBlock* preHeader;
  BasicBlock* header;
  BasicBlock* latch;
  uint32_t depth;

  LoopInfo(BasicBlock* header_) : header(header_), parentLoop(nullptr) {}

  // Get the most outside loop
  LoopInfo* getRootLoop();
  bool containBlock(BasicBlock* block) { return blocks.count(block); }
  bool containBlockInChildren(BasicBlock* block);

  void addBlock(BasicBlock* block);
  void setParentLoop(LoopInfo* parent) { parentLoop = parent; }

  void addSubLoop(LoopInfo* subloop);

  string getName() { return header->getName() + "Loop"; }

  void dump();
};

struct LoopInfoBase {
  vector<LoopInfo*> loopInfos;
  unordered_map<BasicBlock*, LoopInfo*> bbToLoop;

  LoopInfo* getLoopOf(BasicBlock* block);
  void addLoopInfo(LoopInfo* loopInfo);
  void addBlockToLoop(BasicBlock* block, LoopInfo* loopInfo);
  void calculateDepth();
  uint32_t getDepth(BasicBlock* block);
  void dump();
};

#endif
