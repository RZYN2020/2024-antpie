/**
 * A container for the result of dataflow analysis
 */

#ifndef _DATAFLOWRESULT_H_
#define _DATAFLOWANALYSIS_H_

#include <map>

#include "BasicBlock.hh"
using std::map;

template <typename Node, typename Fact>
class DataflowResult {
 private:
  map<Node, Fact> inFacts;
  map<Node, Fact> outFacts;

 public:
  DataflowResult();
  ~DataflowResult();

  Fact getInFact(Node node) { return inFacts[node]; }
  Fact getOutFact(Node node) { return outFacts[node]; }
  void setInFact(Node node, Fact fact) { inFacts[node] = fact; }
  void setOutFact(Node node, Fact fact) { outFacts[node] = fact; }
};

template <typename Node, typename Fact>
DataflowResult<Node, Fact>::DataflowResult() {}

template <typename Node, typename Fact>
DataflowResult<Node, Fact>::~DataflowResult() {
  for (auto& item : inFacts) {
    delete item.second;
  }
  for (auto& item : outFacts) {
    delete item.second;
  }
}

#endif