#ifndef _MEMToREG_H_
#define _MEMToREG_H_
#include <map>

#include "Function.hh"

using std::map;

struct ValueInfo;
class MemToReg {
 private:
  Function* function = 0;
  map<Instruction*, ValueInfo*> instToValueInfo;
  LinkedList<Instruction*> trashList;
  LinkedList<ValueInfo*> valueInfos;
  void runPass();
  bool linkDefsAndUsesToVar(ValueInfo* valueInfo);
  void renameRecursive(BasicBlock* bb);

 public:
  MemToReg(Function* func) : function(func) { runPass(); }
};

#endif