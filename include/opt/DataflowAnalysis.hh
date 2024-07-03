/**
 * Template for defining dataflow analysis
 */
#ifndef _DATAFLOWANALYSIS_H_
#define _DATAFLOWANALYSIS_H_

#include "CFG.hh"

template <typename Fact>
class DataflowAnalysis {
 public:
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