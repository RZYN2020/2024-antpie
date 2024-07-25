/**
 * Loop Unroll
 */
#ifndef _LOOP_UNROLL_H_
#define _LOOP_UNROLL_H_

#include "LoopInfo.hh"
#include "Optimization.hh"

#define UNROLL_FACTOR 8  // Unroll time
#define MAX_LINE 20

struct ClonedLoop {
  LoopInfo* originLoop;
  unordered_map<Value*, Value*> valueMap;

  ClonedLoop(LoopInfo* originLoop_) : originLoop(originLoop_) {}
  BasicBlock* getHeader() {
    return (BasicBlock*)valueMap.at(originLoop->header);
  }
  Value* getNewValue(Value* oldValue) { return valueMap.at(oldValue); }
};

class LoopUnroll : public Optimization {
 private:
  ClonedLoop* cloneLoop(LoopInfo* originLoop);
  bool runOnLoop(LoopInfo* loopInfo);
  uint32_t collectBlocksAndCountLine(LoopInfo* loopInfo,
                                     unordered_set<BasicBlock*>& blocks);
  bool constantOrInvariant(Value* value, LoopInfo* loopInfo);

 public:
  LoopUnroll() {}
  bool runOnModule(ANTPIE::Module* module) override;
  bool runOnFunction(Function* func) override;
};

#endif