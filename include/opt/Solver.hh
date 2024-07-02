/**
 * Dataflow analysis solver
 */
#ifndef _SOLVER_H_
#define _SOLVER_H_

#include <queue>

#include "DataflowAnalysis.hh"
#include "DataflowResult.hh"
using std::queue;

template <typename Fact>
class Solver {
 private:
  DataflowAnalysis<Fact> analysis;

 public:
  Solver()(DataflowAnalysis* analysis_) : analysis(analysis_);

  DataflowResult<BasicBlock*, Fact>* solve(CFG* cfg);

  DataflowResult<BasicBlock*, Fact>* initialize(CFG* cfg);

  void initializeForward(CFG* cfg, DataflowResult<BasicBlock*, Fact>* result);

  void initializeBackward(CFG* cfg, DataflowResult<BasicBlock*, Fact>* result);

  void doSolve(CFG* cfg, DataflowResult<BasicBlock*, Fact>* result);

  void doSolveForward(CFG* cfg, DataflowResult<BasicBlock*, Fact>* result);

  void doSolveBackward(CFG* cfg, DataflowResult<BasicBlock*, Fact>* result);
};


template <typename Fact>
DataflowResult<BasicBlock*, Fact>* Solver<Fact>::solve(CFG* cfg) {
  DataflowResult<BasicBlock*, Fact>* result = initialize(cfg);
  doSolve(cfg, result);
  return result;
}

template <typename Fact>
DataflowResult<BasicBlock*, Fact>* Solver<Fact>::initialize(CFG* cfg) {
  DataflowResult<BasicBlock*, Fact>* result =
      new DataflowResult<BasicBlock*, Fact>();
  if (analysis->isForward()) {
    initializeForward(cfg, result);
  } else {
    initializeBackward(cfg, result);
  }
  return result;
}

template <typename Fact>
void Solver<Fact>::initializeForward(
    CFG* cfg, DataflowResult<BasicBlock*, Fact>* result) {
  for (BasicBlock* bb : cfg->getBlocks()) {
    result->setInFact(bb, analysis.newInitialFact());
    result->setOutFact(bb, analysis.newInitialFact());
  }
  result->setInFact(cfg->getEntry(), analysis->newBoundaryFact());
  result->setOutFact(cfg->getEntry(), analysis->newBoundaryFact());
}

template <typename Fact>
void Solver<Fact>::initializeBackward(
    CFG* cfg, DataflowResult<BasicBlock*, Fact>* result) {
  for (BasicBlock* bb : cfg->getBlocks()) {
    result->setInFact(bb, analysis.newInitialFact());
    result->setOutFact(bb, analysis.newInitialFact());
  }
  result->setInFact(cfg->getExit(), analysis->newBoundaryFact());
  result->setOutFact(cfg->getExit(), analysis->newBoundaryFact());
}

template <typename Fact>
void Solver<Fact>::doSolve(CFG* cfg,
                           DataflowResult<BasicBlock*, Fact>* result) {
  if (analysis->isForward()) {
    doSolveForward(cfg, result);
  } else {
    doSolveBackward(cfg, result);
  }
}

template <typename Fact>
void Solver<Fact>::doSolveForward(CFG* cfg, DataflowResult<BasicBlock*, Fact>* result) {
  queue<BasicBlock*> workList;
  for (BasicBlock* bb : cfg->getBlocks()) {
    workList.push(bb);
  }
  while (!workList.empty()) {
    BasicBlock* bb = workList.front();
    workList.pop();
    Fact inFact = result->getInFact(bb);
    Fact outFact = result->getOutFact(bb);
    for (BasicBlock* pre : cfg->getPredOf(bb)) {
      analysis->meetInto(result->getOutFact(pre), inFact);
    }
    if (analysis->transferBlock(bb, inFact, outFact)) {
      for (BasicBlock* succ : cfg->getSuccOf(bb)) {
        workList.push(succ);
      }
    }
  }
}

template <typename Fact>
void Solver<Fact>::doSolveBackward(CFG* cfg, DataflowResult<BasicBlock*, Fact>* result) {
  queue<BasicBlock*> workList;
  for (BasicBlock* bb : cfg->getBlocks()) {
    workList.push(bb);
  }
  while (!workList.empty()) {
    BasicBlock* bb = workList.front();
    workList.pop();
    Fact inFact = result->getInFact(bb);
    Fact outFact = result->getOutFact(bb);
    for (BasicBlock* succ : cfg->getSuccOf(bb)) {
      analysis->meetInto(result->getOutFact(succ), outFact);
    }
    if (analysis->transferBlock(bb, inFact, outFact)) {
      for (BasicBlock* pre : cfg->getPredOf(bb)) {
        workList.push(pre);
      }
    }
  }
}

#endif