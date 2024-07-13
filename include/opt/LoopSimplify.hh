#ifndef _LOOPSIMPLIFY_H_
#define _LOOPSIMPLIFY_H_

#include "LoopInfo.hh"
#include "Optimization.hh"

class LoopSimplify : public Optimization {
 private:
  bool simplyOneLoop(LoopInfo* loopInfo, CFG* cfg);

 public:
  LoopSimplify() {}
  bool runOnModule(ANTPIE::Module* module) override;
  bool runOnFunction(Function* func) override;
};

#endif