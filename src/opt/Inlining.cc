#include "Inlining.hh"

bool Inlining::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  for (Function* func : *module->getFunctions()) {
    // TODO: check if function can be inline
    changed |= runOnFunction(func);
  }
  return changed;
}

bool Inlining::runOnFunction(Function* func) {
  bool changed = false;
  BBListPtr blockList = func->getBasicBlocks();
  for (auto blockIter = blockList->begin(); blockIter != blockList->end();
       ++blockIter) {
    BasicBlock* block = *blockIter;
    LinkedList<Instruction*>* instructions = block->getInstructions();
    for (auto instrIter = instructions->begin();
         instrIter != instructions->end();) {
      Instruction* instr = *instrIter;
      if (!instr->isa(VT_CALL)) {
        ++instrIter;
        continue;
      }

      CallInst* callInst = dynamic_cast<CallInst*>(instr);
      Function* callee = callInst->getFunction();
      if (callee == func || !needInline(callee)) continue;

      changed = true;

      unordered_map<Value*, Value*> replaceMap;         // old -> new
      unordered_map<Value*, Value*> reverseReplaceMap;  // new -> old
      unordered_map<BasicBlock*, BasicBlock*> blockMap;

      BasicBlock* splitBlock = block->split(instrIter);
      ++instrIter;
      FuncType* funcType = dynamic_cast<FuncType*>(callee->getType());
      int argSize = funcType->getArgSize();
      assert(argSize == callInst->getRValueSize());
      for (int i = 0; i < argSize; i++) {
        replaceMap.emplace(funcType->getArgument(i), callInst->getRValue(i));
      }

      for (BasicBlock* oldBlock : *callee->getBasicBlocks()) {
        BasicBlock* newBlock = oldBlock->clone(replaceMap);
        newBlock->setParent(func);
        blockMap.emplace(oldBlock, newBlock);
        blockList->insertAfter(blockIter, newBlock);
        ++blockIter;
        replaceMap.emplace(oldBlock, newBlock);
      }

      blockList->insertAfter(blockIter, splitBlock);

      // deal with exit
      BasicBlock* newExit = blockMap[callee->getExit()];
      PhiInst* retPhi = 0;
      if (callInst->getType()->getTypeTag() != TT_VOID) {
        retPhi = new PhiInst(callInst->getName() + "_phi");
        newExit->pushInstr(retPhi);
        callInst->replaceAllUsesWith(retPhi);
      }
      newExit->pushInstr(new JumpInst(splitBlock));

      // deal with entry
      callInst->eraseFromParent();
      callInst->deleteUseList();
      delete callInst;
      block->pushInstr(new JumpInst(blockMap[callee->getEntry()]));

      for (auto [oldValue, newValue] : replaceMap) {
        reverseReplaceMap.emplace(newValue, oldValue);
      }

      for (auto [oBlock, nBlock] : blockMap) {
        LinkedList<Instruction*>* nInstrList = nBlock->getInstructions();
        for (auto it = nInstrList->begin(); it != nInstrList->end();) {
          Instruction* nInstr = *it;
          Instruction* oInstr =
              dynamic_cast<Instruction*>(reverseReplaceMap[nInstr]);
          if (!oInstr) {
            ++it;
            continue;
          }
          nInstr->cloneUseList(replaceMap, oInstr->getUseList());

          // deal with return value
          if (nInstr->isa(VT_RET)) {
            ReturnInst* retInstr = dynamic_cast<ReturnInst*>(nInstr);
            if (retPhi) {
              retPhi->pushIncoming(retInstr->getRValue(0), nBlock);
            }
            ++it;
            nInstr->eraseFromParent();
            nInstr->deleteUseList();
            delete nInstr;
            nBlock->pushInstr(new JumpInst(newExit));

          } else if (nInstr->isa(VT_ALLOCA)) {
            // move alloca to entry block
            ++it;
            nInstr->eraseFromParent();
            func->getEntry()->pushInstrAtHead(nInstr);

          } else {
            ++it;
          }
        }
      }
    }
  }
  if (changed) {
    func->resetCFG();
    func->resetDT();
  }
  return changed;
}

bool Inlining::needInline(Function* func) {
  return !func->isRecursive() && func->getLineSize() < INLINE_THRESHOLD;
}
