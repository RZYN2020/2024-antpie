/**
 * Store Elimination
 */
#ifndef _STORE_ELIMINATION_H_
#define _STORE_ELIMINATION_H_

#include "Optimization.hh"

class StoreElimination : public Optimization {
 private:
  bool runOnBasicBlock(BasicBlock* block);

 public:
  StoreElimination() {}
  bool runOnModule(ANTPIE::Module* module) override;
  bool runOnFunction(Function* func) override;
};

#endif