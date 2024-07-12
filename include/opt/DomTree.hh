#ifndef _DOMTREE_H_
#define _DOMTREE_H_

#include <assert.h>

#include <functional>
#include <map>
#include <queue>
#include <unordered_set>

#include "BasicBlock.hh"
#include "CFG.hh"
#include "VisualizeGraph.hh"

using std::map;
using std::queue;
using std::unordered_set;

class Function;

class DomTree {
 private:
  const LinkedList<BasicBlock*>* blocks;
  CFG* cfg;
  map<BasicBlock*, BasicBlock*> dominators;
  map<BasicBlock*, BBListPtr> domChildren;
  map<BasicBlock*, BBListPtr> dominanceFrontier;
  bool dtActive = 0;
  bool dfActive = 0;
  vector<BasicBlock*> dfnToBB;
  map<BasicBlock*, int> bbToDfn;
  vector<int> iDom;
  void dfs(BasicBlock* node, int& d, CFG* cfg);

 public:
  DomTree(Function* func);
  DomTree() : dtActive(false) {}

  bool dtReady() { return dtActive; }
  bool dfReady() { return dfActive; }

  void setDominator(BasicBlock* bb, BasicBlock* domNode) {
    dominators[bb] = domNode;
  }
  void buildDomTree();
  BasicBlock* getDominator(BasicBlock* bb) { return dominators.at(bb); }
  BBListPtr getDomChildren(BasicBlock* bb) { return domChildren[bb]; }
  bool dominates(BasicBlock* parent, BasicBlock* block);
  BBListPtr postOrder();
  void testDomTree();

  void calculateDF();
  BBListPtr getDF(BasicBlock* bb) const { return dominanceFrontier.at(bb); }

  void mergeChildrenTo(BasicBlock* src, BasicBlock* dest);
  void deleteChildren(BasicBlock* block) { domChildren[block]->clear(); }
  void deleteParent(BasicBlock* block) { dominators.erase(block); }
  void draw();

  void calculateIDF(BBListPtr src, BBListPtr result);
};

#endif
