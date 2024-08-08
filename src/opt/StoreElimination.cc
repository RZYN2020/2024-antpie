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
  AliasAnalysisResult* aaResult = func->getAliasAnalysisResult();
  assert(aaResult);
  for (BasicBlock* block : *func->getBasicBlocks()) {
    changed |= runOnBasicBlock(block, aaResult);
  }
  return changed;
}

bool StoreElimination::runOnBasicBlock(BasicBlock* block,
                                       AliasAnalysisResult* aaResult) {
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
        unordered_set<Value*> eraseSet;
        for (auto& [preMem, preStore] : memToStore) {
          if (!aaResult->isDistinct(preMem, mem)) {
            eraseSet.insert(preMem);
          }
        }
        for (auto& eraseMem : eraseSet) {
          memToStore.erase(eraseMem);
        }
        break;
      }
      case VT_CALL: {
        memToStore.clear();
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