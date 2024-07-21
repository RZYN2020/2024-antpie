#include "GlobalCodeMotion.h"

bool GlobalCodeMotion::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  for (Function* function : *module->getFunctions()) {
    changed |= runOnFunction(function);
  }
  return changed;
}

bool GlobalCodeMotion::runOnFunction(Function* func) {
  DomTree* dt = func->getDT();
  if (!dt) dt = func->buildDT();
  dt->calculateDepth();

  // 0. init
  // TODO: GC?
  infoMap.clear();
  for (BasicBlock* bb : *func->getBasicBlocks()) {
    for (Instruction* instr : *bb->getInstructions()) {
      GCMInfo* newInfo = new GCMInfo();
      if (isPinned(instr)) {
        newInfo->earliestBlock = newInfo->latestBlock = bb;
        newInfo->bestBlock = bb;
      }
      infoMap[instr] = newInfo;
    }
  }

  // 1. Sechedule early
  visited.clear();
  for (BasicBlock* bb : *func->getBasicBlocks()) {
    for (Instruction* instr : *bb->getInstructions()) {
      if (isPinned(instr)) {
        visited.insert(instr);
        int opSize = instr->getRValueSize();
        for (int i = 0; i < opSize; i++) {
          Value* rValue = instr->getRValue(i);
          Instruction* opInstr = nullptr;
          if (opInstr = dynamic_cast<Instruction*>(rValue)) {
            scheduleEarly(opInstr, dt);
          }
        }
      }
    }
  }

  // 2. Schedule late
  LoopInfoBase* liBase = func->getLoopInfoBase();
  liBase->calculateDepth();
  visited.clear();
  for (BasicBlock* bb : *func->getBasicBlocks()) {
    auto instrList = bb->getInstructions();
    for (auto it = instrList->begin(); it != instrList->end();) {
      Instruction* instr = *it;
      ++it;
      if (isPinned(instr)) {
        visited.insert(instr);
        Use* use = instr->getUseHead();
        while (use) {
          Instruction* userInstr = use->instr;
          scheduleLate(userInstr, dt, liBase);
          use = use->next;
        }
      }
    }
  }
  for (BasicBlock* bb : *func->getBasicBlocks()) {
    auto instrList = bb->getInstructions();
    for (auto it = instrList->begin(); it != instrList->end();) {
      Instruction* instr = *it;
      ++it;
      if (!visited.count(instr)) {
        scheduleLate(instr, dt, liBase);
      }
    }
  }
  return true;
}

bool GlobalCodeMotion::isPinned(Instruction* instr) {
  return instr->isa(VT_JUMP) || instr->isa(VT_BR) || instr->isa(VT_RET) ||
         instr->isa(VT_CALL) || instr->isa(VT_PHI) || instr->isa(VT_LOAD) ||
         instr->isa(VT_STORE) || instr->isa(VT_ALLOCA);
}

Instruction* GlobalCodeMotion::scheduleEarly(Instruction* instr, DomTree* dt) {
  if (visited.count(instr)) {
    return instr;
  }
  visited.insert(instr);
  BasicBlock* block = dt->getRoot();
  int opSize = instr->getRValueSize();

  for (int i = 0; i < opSize; i++) {
    Value* rValue = instr->getRValue(i);
    Instruction* opInstr = nullptr;
    if (opInstr = dynamic_cast<Instruction*>(rValue)) {
      opInstr = scheduleEarly(opInstr, dt);
      BasicBlock* earliesBlock = infoMap[opInstr]->earliestBlock;
      if (dt->getDepth(block) < dt->getDepth(earliesBlock)) {
        block = earliesBlock;
      }
    }
  }
  if (!isPinned(instr)) {
    infoMap[instr]->earliestBlock = block;
  }
  return instr;
}

Instruction* GlobalCodeMotion::scheduleLate(Instruction* instr, DomTree* dt,
                                            LoopInfoBase* liBase) {
  if (visited.count(instr)) {
    return instr;
  }
  visited.insert(instr);
  BasicBlock* lca = nullptr;

  for (Use* use = instr->getUseHead(); use; use = use->next) {
    Instruction* userInstr = use->instr;
    userInstr = scheduleLate(userInstr, dt, liBase);
    BasicBlock* useBlock = userInstr->getParent();
    if (PhiInst* phiInst = dynamic_cast<PhiInst*>(userInstr)) {
      int icSize = phiInst->getRValueSize() / 2;
      for (int i = 0; i < icSize; i++) {
        if (phiInst->getRValue(i * 2) == (Value*)instr) {
          useBlock = (BasicBlock*)phiInst->getRValue(i * 2 + 1);
          if (lca) {
            lca = dt->findLCA(lca, useBlock);
          } else {
            lca = useBlock;
          }
        }
      }
    }

    if (lca) {
      lca = dt->findLCA(lca, useBlock);
    } else {
      lca = useBlock;
    }
  }

  if (isPinned(instr)) {
    return instr;
  }
  // Should not have dead code
  // assert(lca);
  if (!lca) return instr;

  // Find the best location
  BasicBlock* bestBlock = lca;
  BasicBlock* earlestBlock = infoMap[instr]->earliestBlock;
  while (lca != earlestBlock) {
    if (liBase->getDepth(lca) < liBase->getDepth(bestBlock)) {
      bestBlock = lca;
    }
    lca = dt->getDominator(lca);
  }
  if (liBase->getDepth(lca) < liBase->getDepth(bestBlock)) {
    bestBlock = lca;
  }

  // Move instruction
  infoMap[instr]->latestBlock = bestBlock;
  if (bestBlock == instr->getParent()) return instr;
  for (Use* use = instr->getUseHead(); use; use = use->next) {
    Instruction* userInstr = use->instr;
    if (userInstr->getParent() == bestBlock && !instr->isa(VT_PHI)) {
      instr->moveBefore(userInstr);
      return instr;
    }
  }

  if (bestBlock->getTailInstr()) {
    instr->moveBefore(bestBlock->getTailInstr());
  } else {
    bestBlock->pushInstr(instr);
  }
  return instr;
}