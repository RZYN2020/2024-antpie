/**
 * Template for defining dataflow analysis
 */
#ifndef _DATAFLOWANALYSIS_H_
#define _DATAFLOWANALYSIS_H_

#include "CFG.hh"

template <typename Fact>
class DataflowAnalysis {
 public:
    struct {
        void (*teardown) (DataflowAnalysis *t);
        bool (*isForward) (DataflowAnalysis *t);
        Fact *(*newBoundaryFact) (DataflowAnalysis *t, Function *func);
        Fact *(*newInitialFact) (DataflowAnalysis *t);
        void (*setInFact) (DataflowAnalysis *t, BasicBlock *blk, Fact *fact);
        void (*setOutFact) (DataflowAnalysis *t, BasicBlock *blk, Fact *fact);
        Fact *(*getInFact) (DataflowAnalysis *t, BasicBlock *blk);
        Fact *(*getOutFact) (DataflowAnalysis *t, BasicBlock *blk);
        bool (*meetInto) (DataflowAnalysis *t, Fact *fact, Fact *target);
        bool (*transferBlock) (DataflowAnalysis *t, BasicBlock *block, Fact *in_fact, Fact *out_fact);
        void (*printResult) (DataflowAnalysis *t, BasicBlock *func);
    } const *vTable;

  // return true if analysis is forward
  virtual bool isForward() = 0;

  // return new boundary fact, 
  // i.e., the fact for entry(exit) in forward (backward) analysis
  virtual Fact newBoundaryFact(CFG* cfg);

  // return new initial fact for non-boundary nodes
  virtual Fact newInitialFact();

  // meet fact and target into target
  virtual void meetInto(Fact fact, Fact target);

  // transfer function
  virtual bool transferBlock(BasicBlock* bb, Fact in, Fact out);
};




#endif