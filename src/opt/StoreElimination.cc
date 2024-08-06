#include "StoreElimination.hh"

bool StoreElimination::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  for (Function* func : *module->getFunctions()) {
    changed |= runOnFunction(func);
  }
  return changed;
}

bool StoreElimination::runOnFunction(Function* func) {
  bool changed = false;
  for (BasicBlock* block : *func->getBasicBlocks()) {
    changed |= runOnBasicBlock(block);
  }
  return changed;
}

bool StoreElimination::runOnBasicBlock(BasicBlock* block) {
  unordered_map<Value*, Instruction*> memToStore;
  unordered_set<Instruction*> trashStores;
  for (Instruction* instr : *block->getInstructions()) {
    switch (instr->getValueTag()) {
      case VT_STORE: {
        Value* mem = instr->getRValue(1);
        auto it = memToStore.find(mem);
        if (it != memToStore.end()) {
          trashStores.insert(it->second);
          memToStore[mem] = instr;
        } else {
          memToStore[mem] = instr;
        }
        break;
      }
      case VT_LOAD: {
        Value* mem = instr->getRValue(0);
        memToStore.erase(mem);
        break;
      }
      case VT_CALL: {
        CallInst* callInst = (CallInst*)instr;
        Function* callee = callInst->getFunction();
        if (callee->hasMemRead()) {
          memToStore.clear();
        }
        break;
      }
    }
  }

  for (Instruction* trash : trashStores) {
    trash->eraseFromParent();
    trash->deleteUseList();
  }
  return !trashStores.empty();
}