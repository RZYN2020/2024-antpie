#ifndef _LOOPINFO_H_
#define _LOOPINFO_H_

#include <functional>
#include <unordered_set>
#include <vector>

#include "BasicBlock.hh"
using std::unordered_set;
using std::vector;

struct SimpleLoopInfo;
struct LoopInfoBase;

struct LoopInfo {
  LoopInfoBase* liBase;
  LoopInfo* parentLoop;
  vector<LoopInfo*> subLoops;
  unordered_set<BasicBlock*> blocks;

  BasicBlock* preHeader;
  BasicBlock* header;
  unordered_set<BasicBlock*> latches;
  unordered_set<BasicBlock*> exitings;
  unordered_set<BasicBlock*> exits;

  uint32_t depth;

  SimpleLoopInfo* simpleLoop;

  LoopInfo(BasicBlock* header_)
      : header(header_), parentLoop(0), simpleLoop(0) {}

  // Get the most outside loop
  LoopInfo* getRootLoop();
  bool containBlock(BasicBlock* block) { return blocks.count(block); }
  bool containBlockInChildren(BasicBlock* block);

  void addBlock(BasicBlock* block);
  void setParentLoop(LoopInfo* parent) { parentLoop = parent; }
  void addSubLoop(LoopInfo* subloop);
  void setLatches(vector<BasicBlock*>& latches_);
  void addLatch(BasicBlock* latch) { latches.insert(latch); }
  void addExiting(BasicBlock* exiting) { exitings.insert(exiting); }
  void addExit(BasicBlock* exit) { exits.insert(exit); }

  bool isSimpleLoop() { return simpleLoop != nullptr; }
  void analyseSimpleLoop();

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
  void analyseSimpleLoop();
  void dump();
};

/**
 * SimpleLoop:
 * 1. only one header, one exiting, one latch
 * 2. header == exiting
 * 3. Loop is control by an induction variable,
 *    which changed in a PHI and a ALU
 */

struct SimpleLoopInfo {
  Value* initValue;
  PhiInst* phiInstr;
  BinaryOpInst* strideInstr;
  BranchInst* brInstr;

  bool pureIdv;
};

#endif
