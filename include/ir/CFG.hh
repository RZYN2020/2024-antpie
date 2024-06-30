#ifndef _CFG_H_
#define _CFG_H_

#include <map>

#include "Function.hh"
using std::map;

typedef LinkedList<BasicBlock*>* BBListPtr;

class CFG {
 private:
  LinkedList<BasicBlock*> blocks;
  BasicBlock *entry, *exit;
  map<BasicBlock*, BBListPtr> blkPredMap;
  map<BasicBlock*, BBListPtr> blkSuccMap;

  void addEdge(BasicBlock* src, BasicBlock* dest);

 public:
  CFG(Function* func);

  BBListPtr getPredOf(BasicBlock* bb) { return blkPredMap[bb]; }
  BBListPtr getSuccOf(BasicBlock* bb) { return blkSuccMap[bb]; }
  BBListPtr getBlocks() { return &blocks; }

  void debug();
};

#endif