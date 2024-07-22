#include "DeadCodeElimination.hh"

bool DeadCodeElimination::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  for (Function* function : *module->getFunctions()) {
    changed |= runOnFunction(function);
  }
  return changed;
}

bool DeadCodeElimination::runOnFunction(Function* func) {
  bool changed = false;
  changed |= simplifyInstruction(func);
  changed |= eliminateDeadBlocks(func);
  changed |= eliminateDeadInstructions(func);
  return changed;
}

bool DeadCodeElimination::eliminateDeadBlocks(Function* func) {
  CFG* cfg = func->getCFG();
  if (!cfg) cfg = func->buildCFG();
  unordered_set<BasicBlock*> deadBlocks;
  for (BasicBlock* block : *func->getBasicBlocks()) {
    deadBlocks.insert(block);
  }

  // bfs on cfg
  LinkedList<BasicBlock*> worklist;
  worklist.pushBack(cfg->getEntry());
  while (!worklist.isEmpty()) {
    BasicBlock* block = worklist.popFront();
    if (deadBlocks.count(block) == 0) continue;
    for (BasicBlock* succ : *cfg->getSuccOf(block)) {
      worklist.pushBack(succ);
    }
    deadBlocks.erase(block);
  }

  for (BasicBlock* deadBlock : deadBlocks) {
    if (deadBlock == func->getExit()) continue;

    for (Instruction* instr : *deadBlock->getInstructions()) {
      instr->deleteUseList();
    }
    cfg->eraseNode(deadBlock);
    deadBlock->eraseFromParent();
    delete deadBlock;
  }
  return !deadBlocks.empty();
}
Value* tmp = 0;
bool DeadCodeElimination::eliminateDeadInstructions(Function* func) {
  LinkedList<Instruction*> worklist;
  for (BasicBlock* block : *func->getBasicBlocks()) {
    for (Instruction* instr : *block->getInstructions()) {
      if (!instr->getUseHead() && isComputeInstruction(instr)) {
        // No user
        worklist.pushBack(instr);
      }
    }
  }
  bool changed = !worklist.isEmpty();

  LinkedList<Instruction*> trashList;

  while (!worklist.isEmpty()) {
    Instruction* deadInst = worklist.popFront();
    if (deadInst->getUseHead() || !deadInst->getParent()) continue;
    int argSize = deadInst->getRValueSize();
    for (int i = 0; i < argSize; i++) {
      Value* rValue = deadInst->getRValue(i);
      tmp = rValue;
      if (Instruction* effectInst = dynamic_cast<Instruction*>(rValue)) {
        if (isComputeInstruction(effectInst)) {
          worklist.pushBack(effectInst);
        }
      }
    }
    deadInst->eraseFromParent();
    deadInst->deleteUseList();
    trashList.pushBack(deadInst);
  }
  for (Instruction* trash : trashList) {
    delete trash;
  }

  return changed;
}

bool DeadCodeElimination::simplifyInstruction(Function* func) {
  bool changed = false;
  for (BasicBlock* block : *func->getBasicBlocks()) {
    // Cond branch to uncond branch
    if (BranchInst* branch = dynamic_cast<BranchInst*>(block->getTailInstr())) {
      Value* cond = branch->getRValue(0);
      if (BoolConstant* boolConstant = dynamic_cast<BoolConstant*>(cond)) {
        changed = true;
        BasicBlock* succBlock = nullptr;
        BasicBlock* unreachBLock = nullptr;
        if (boolConstant->getValue()) {
          succBlock = (BasicBlock*)branch->getRValue(1);
          unreachBLock = (BasicBlock*)branch->getRValue(2);
        } else {
          succBlock = (BasicBlock*)branch->getRValue(2);
          unreachBLock = (BasicBlock*)branch->getRValue(1);
        }
        if (func->getCFG() && unreachBLock != succBlock) {
          func->getCFG()->eraseEdge(block, unreachBLock);
        }
        branch->eraseFromParent();
        branch->deleteUseList();
        block->pushInstr(new JumpInst(succBlock));
        delete branch;

        // delete phi incoming from block to unreachBlock
        for (Instruction* instr : *unreachBLock->getInstructions()) {
          if (PhiInst* phiInstr = dynamic_cast<PhiInst*>(instr)) {
            phiInstr->deleteIncomingFrom(block);
          }
        }
      }
    }

    // delete single entry phi
    PhiInst* phiInst = 0;
    for (auto it = block->getInstructions()->begin();
         it != block->getInstructions()->end();) {
      Instruction* instr = *it;
      ++it;
      if ((phiInst = dynamic_cast<PhiInst*>(instr)) &&
          (instr->getRValueSize() == 2)) {
        changed = true;
        phiInst->replaceAllUsesWith(phiInst->getRValue(0));
        phiInst->eraseFromParent();
        phiInst->deleteUseList();
      } else {
        break;
      }
    }
  }
  return changed;
}

bool DeadCodeElimination::isComputeInstruction(Instruction* instr) {
  if (instr->isa(VT_JUMP) || instr->isa(VT_BR) || instr->isa(VT_RET) ||
      instr->isa(VT_STORE) || instr->isa(VT_CALL)) {
    return false;
  }
  return true;
}
