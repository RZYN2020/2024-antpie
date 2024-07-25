#include "LoopUnroll.hh"

// Note: This code is untested and may contain bugs.

ClonedLoop* LoopUnroll::cloneLoop(LoopInfo* originLoop) {
  ClonedLoop* clonedLoop = new ClonedLoop(originLoop);
  unordered_map<Value*, Value*>& valueMap = clonedLoop->valueMap;

  LinkedList<LoopInfo*> bfsList;
  bfsList.pushBack(originLoop);
  while (!bfsList.isEmpty()) {
    LoopInfo* dealLoop = bfsList.popFront();
    for (BasicBlock* oldBlock : dealLoop->blocks) {
      BasicBlock* newBlock = oldBlock->clone(valueMap);
      valueMap.emplace(oldBlock, newBlock);
    }
    for (LoopInfo* subloop : dealLoop->subLoops) {
      bfsList.pushBack(subloop);
    }
  }
  return clonedLoop;
}

bool LoopUnroll::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  for (Function* function : *module->getFunctions()) {
    changed |= runOnFunction(function);
  }
  return changed;
}

bool LoopUnroll::runOnFunction(Function* func) {
  bool changed = false;
  LoopInfoBase* liBase = func->getLoopInfoBase();
  liBase->analyseSimpleLoop();
  for (LoopInfo* loopInfo : liBase->loopInfos) {
    if (loopInfo->subLoops.empty()) {
      changed |= runOnLoop(loopInfo);
    }
  }
  if (changed) {
    func->resetCFG();
    func->resetDT();
  }
  return changed;
}

bool LoopUnroll::runOnLoop(LoopInfo* loopInfo) {
  bool changed = false;
  for (LoopInfo* subLoop : loopInfo->subLoops) {
    changed |= runOnLoop(subLoop);
  }

  if (!loopInfo->isSimpleLoop()) return changed;
  unordered_set<BasicBlock*> loopBlocks;
  uint32_t size = collectBlocksAndCountLine(loopInfo, loopBlocks);

  // Too big
  if (size > MAX_LINE) return changed;

  SimpleLoopInfo* simpleLoop = loopInfo->simpleLoop;
  BranchInst* brInst = simpleLoop->brInstr;
  Value* initValue = simpleLoop->initValue;
  BinaryOpInst* strideInstr = simpleLoop->strideInstr;
  Value* stride = strideInstr->getRValue(1);
  Instruction* condInstr = (Instruction*)simpleLoop->brInstr->getRValue(0);
  Value* endValue = condInstr->getRValue(1);
  // check if init, stride is constant
  // Consider: if initValue is a single entry PHI?
  if (!constantOrInvariant(initValue, loopInfo)) {
    return changed;
  }
  if (!dynamic_cast<Constant*>(stride)) {
    return changed;
  }
  // check end is constant or loop invariant
  if (!constantOrInvariant(endValue, loopInfo)) return changed;
  // Modify head branch
  BinaryOpInst* endModify = 0;
  bool deadLoop = false;
  switch (strideInstr->getOpTag()) {
    case ADD: {
      IntegerConstant* intStrideInstr = dynamic_cast<IntegerConstant*>(stride);
      int strideInt = intStrideInstr->getValue();
      if (IcmpInst* icmpInst = dynamic_cast<IcmpInst*>(condInstr)) {
        switch (icmpInst->getOpTag()) {
          case SLE:
          case SGE:
          case SLT:
          case SGT: {
            if ((int64_t)strideInt * UNROLL_FACTOR > INT32_MAX) return changed;
            endModify = new BinaryOpInst(
                SUB, endValue,
                IntegerConstant::getConstInt(strideInt * UNROLL_FACTOR),
                LabelManager::getLabel(stride->getName()));
            break;
          }

          default:
            return changed;
        }
      }
      break;
    }
    case SUB: {
      IntegerConstant* intStrideInstr = dynamic_cast<IntegerConstant*>(stride);
      int strideInt = intStrideInstr->getValue();
      if (IcmpInst* icmpInst = dynamic_cast<IcmpInst*>(condInstr)) {
        switch (icmpInst->getOpTag()) {
          case SLE:
          case SGE:
          case SLT:
          case SGT: {
            if ((int64_t)strideInt * UNROLL_FACTOR > INT32_MAX) return changed;
            endModify = new BinaryOpInst(
                ADD, endValue,
                IntegerConstant::getConstInt(strideInt * UNROLL_FACTOR),
                LabelManager::getLabel(stride->getName()));
            break;
          }
          default:
            return changed;
        }
      }
      break;
    }
    case MUL: {
      IntegerConstant* intStrideInstr = dynamic_cast<IntegerConstant*>(stride);
      int strideInt = intStrideInstr->getValue();
      if (strideInt <= 0) return changed;
      if (IcmpInst* icmpInst = dynamic_cast<IcmpInst*>(condInstr)) {
        switch (icmpInst->getOpTag()) {
          case SLE:
          case SGE:
          case SLT:
          case SGT: {
            uint64_t newStride = (uint64_t)strideInt * UNROLL_FACTOR;
            if (newStride > INT32_MAX) return changed;
            endModify = new BinaryOpInst(
                SDIV, endValue, IntegerConstant::getConstInt(newStride),
                LabelManager::getLabel(stride->getName()));
            break;
          }
          default:
            return changed;
        }
      }
      break;
    }
    case FADD: {
      FloatConstant* floatStrideInstr = dynamic_cast<FloatConstant*>(stride);
      float strideFloat = floatStrideInstr->getValue();
      if (FcmpInst* fcmpInst = dynamic_cast<FcmpInst*>(condInstr)) {
        switch (fcmpInst->getOpTag()) {
          case OLE:
          case OGE:
          case OLT:
          case OGT: {
            endModify = new BinaryOpInst(
                FSUB, endValue,
                FloatConstant::getConstFloat(strideFloat * UNROLL_FACTOR),
                LabelManager::getLabel(stride->getName()));
            break;
          }

          default:
            return changed;
        }
      }
      break;
    }
    case FSUB: {
      FloatConstant* floatStrideInstr = dynamic_cast<FloatConstant*>(stride);
      float strideFloat = floatStrideInstr->getValue();
      if (FcmpInst* fcmpInst = dynamic_cast<FcmpInst*>(condInstr)) {
        switch (fcmpInst->getOpTag()) {
          case OLE:
          case OGE:
          case OLT:
          case OGT: {
            endModify = new BinaryOpInst(
                FADD, endValue,
                FloatConstant::getConstFloat(strideFloat * UNROLL_FACTOR),
                LabelManager::getLabel(stride->getName()));
            break;
          }
          default:
            return changed;
        }
      }
      break;
    }
    case FMUL: {
      FloatConstant* floatStrideInstr = dynamic_cast<FloatConstant*>(stride);
      float strideFloat = floatStrideInstr->getValue();
      if (strideFloat <= 0) return changed;
      if (FcmpInst* fcmpInst = dynamic_cast<FcmpInst*>(condInstr)) {
        switch (fcmpInst->getOpTag()) {
          case OLE:
          case OGE:
          case OLT:
          case OGT: {
            float newStride = strideFloat * UNROLL_FACTOR;
            endModify = new BinaryOpInst(
                FDIV, endValue, FloatConstant::getConstFloat(newStride),
                LabelManager::getLabel(stride->getName()));
            break;
          }
          default:
            return changed;
        }
      }
      break;
    }
    default:
      return changed;
  }
  changed = true;

  Function* function = loopInfo->header->getParent();

  // remain loop
  ClonedLoop* remainLoop = cloneLoop(loopInfo);

  // outside block use value defined in loop
  LinkedList<std::pair<Use*, Instruction*>> replaceList;
  for (Instruction* oldInst : *loopInfo->header->getInstructions()) {
    Instruction* newInst = (Instruction*)remainLoop->getNewValue(oldInst);
    for (Use* use = oldInst->getUseHead(); use; use = use->next) {
      Instruction* userInstr = use->instr;
      if (!loopInfo->containBlockInChildren(userInstr->getParent())) {
        replaceList.pushBack({use, newInst});
      }
    }
  }
  for (auto [use, instr]: replaceList) {
    use->replaceValue(instr);
  }
  
  // set up remain loop insstruction
  for (auto [oldValue, newValue] : remainLoop->valueMap) {
    PhiInst* oldPhi = 0;
    if ((oldPhi = dynamic_cast<PhiInst*>(oldValue)) &&
        oldPhi->getParent() == loopInfo->header) {
      PhiInst* newPhi = dynamic_cast<PhiInst*>(newValue);
      int icSize = oldPhi->getRValueSize() / 2;
      for (int i = 0; i < icSize; i++) {
        BasicBlock* icBlock = (BasicBlock*)oldPhi->getRValue(2 * i + 1);
        if (!loopBlocks.count(icBlock)) continue;
        newPhi->pushIncoming(oldPhi, oldPhi->getParent());
        newPhi->pushIncoming(
            remainLoop->getNewValue(oldPhi->getRValue(2 * i)),
            (BasicBlock*)remainLoop->getNewValue(oldPhi->getRValue(2 * i + 1)));
      }
    } else if (Instruction* oldInstr = dynamic_cast<Instruction*>(oldValue)) {
      Instruction* newInstr = dynamic_cast<Instruction*>(newValue);
      newInstr->cloneUseList(remainLoop->valueMap, oldInstr->getUseList());
    } else if (BasicBlock* newBlock = dynamic_cast<BasicBlock*>(newValue)) {
      function->pushBasicBlock(newBlock);
    }
  }
  // origin loop jump out to remain loop
  brInst->replaceDestinationWith(*loopInfo->exits.begin(),
                                 remainLoop->getHeader());

  // First cloned loop

  ClonedLoop* firstCloneLoop = cloneLoop(loopInfo);
  ClonedLoop* lastCloneLoop = firstCloneLoop;
  BasicBlock* newHeader = firstCloneLoop->getHeader();
  unordered_map<Value*, Value*>& valueMap = firstCloneLoop->valueMap;
  for (auto [oldValue, newValue] : valueMap) {
    PhiInst* oldPhi = 0;
    if ((oldPhi = dynamic_cast<PhiInst*>(oldValue)) &&
        oldPhi->getParent() == loopInfo->header) {
      PhiInst* newPhi = static_cast<PhiInst*>(newValue);
      int icSize = oldPhi->getRValueSize() / 2;
      for (int i = icSize - 1; i >= 0; i--) {
        BasicBlock* icBlock = (BasicBlock*)oldPhi->getRValue(i * 2 + 1);
        if (!loopBlocks.count(icBlock)) continue;
        Value* icValue = oldPhi->getRValue(2 * i);
        newPhi->pushIncoming(icValue, icBlock);
      }
    } else if (BranchInst* oldBranch = dynamic_cast<BranchInst*>(oldValue)) {
      oldBranch->replaceDestinationWith(loopInfo->header, newHeader);
      BranchInst* newBranch = dynamic_cast<BranchInst*>(newValue);
      newBranch->cloneUseList(valueMap, oldBranch->getUseList());
    } else if (JumpInst* oldJump = dynamic_cast<JumpInst*>(oldValue)) {
      oldJump->replaceDestinationWith(loopInfo->header, newHeader);
      JumpInst* newJump = dynamic_cast<JumpInst*>(newValue);
      newJump->cloneUseList(valueMap, oldJump->getUseList());
    } else if (Instruction* oldInstr = dynamic_cast<Instruction*>(oldValue)) {
      Instruction* newInstr = dynamic_cast<Instruction*>(newValue);
      newInstr->cloneUseList(valueMap, oldInstr->getUseList());
    } else if (BasicBlock* newBlock = dynamic_cast<BasicBlock*>(newValue)) {
      function->pushBasicBlock(newBlock);
    }
  }
  // Header branch
  BranchInst* tailInstr = (BranchInst*)loopInfo->header->getTailInstr();
  BasicBlock* bodyBlock = nullptr;
  if (loopBlocks.count((BasicBlock*)tailInstr->getRValue(1))) {
    bodyBlock =
        (BasicBlock*)firstCloneLoop->getNewValue(tailInstr->getRValue(1));
  } else {
    bodyBlock =
        (BasicBlock*)firstCloneLoop->getNewValue(tailInstr->getRValue(2));
  }
  Instruction* newBranch = newHeader->getTailInstr();
  newBranch->eraseFromParent();
  newBranch->deleteUseList();
  newHeader->pushInstr(new JumpInst(bodyBlock));

  // rest loop
  unordered_map<Value*, Value*>* preValueMap = &lastCloneLoop->valueMap;
  ClonedLoop* preClonedLoop = lastCloneLoop;
  for (int i = 2; i < UNROLL_FACTOR; i++) {
    lastCloneLoop = cloneLoop(loopInfo);
    newHeader = lastCloneLoop->getHeader();
    unordered_map<Value*, Value*>& newValueMap = lastCloneLoop->valueMap;
    for (auto [oldValue, newValue] : newValueMap) {
      PhiInst* oldPhi = 0;
      if ((oldPhi = dynamic_cast<PhiInst*>(oldValue)) &&
          oldPhi->getParent() == loopInfo->header) {
        PhiInst* newPhi = static_cast<PhiInst*>(newValue);
        int icSize = oldPhi->getRValueSize() / 2;
        for (int i = icSize - 1; i >= 0; i--) {
          BasicBlock* icBlock = (BasicBlock*)oldPhi->getRValue(i * 2 + 1);
          if (!loopBlocks.count(icBlock)) continue;
          Value* icValue = oldPhi->getRValue(2 * i);
          newPhi->pushIncoming(preValueMap->at(icValue),
                               (BasicBlock*)preValueMap->at(icBlock));
        }
      } else if (BranchInst* oldBranch = dynamic_cast<BranchInst*>(oldValue)) {
        BranchInst* preBranch = (BranchInst*)preValueMap->at(oldBranch);
        preBranch->replaceDestinationWith(firstCloneLoop->getHeader(),
                                          newHeader);
        BranchInst* newBranch = dynamic_cast<BranchInst*>(newValue);
        newBranch->cloneUseList(newValueMap, oldBranch->getUseList());
      } else if (JumpInst* oldJump = dynamic_cast<JumpInst*>(oldValue)) {
        JumpInst* preJump = (JumpInst*)preValueMap->at(oldJump);
        preJump->replaceDestinationWith(firstCloneLoop->getHeader(), newHeader);
        JumpInst* newJump = dynamic_cast<JumpInst*>(newValue);
        newJump->cloneUseList(newValueMap, oldJump->getUseList());
      } else if (Instruction* oldInstr = dynamic_cast<Instruction*>(oldValue)) {
        Instruction* newInstr = dynamic_cast<Instruction*>(newValue);
        newInstr->cloneUseList(newValueMap, oldInstr->getUseList());
      } else if (BasicBlock* newBlock = dynamic_cast<BasicBlock*>(newValue)) {
        function->pushBasicBlock(newBlock);
      }
    }
    // Header branch
    if (loopBlocks.count((BasicBlock*)tailInstr->getRValue(1))) {
      bodyBlock =
          (BasicBlock*)lastCloneLoop->getNewValue(tailInstr->getRValue(1));
    } else {
      bodyBlock =
          (BasicBlock*)lastCloneLoop->getNewValue(tailInstr->getRValue(2));
    }

    Instruction* newBranch = newHeader->getTailInstr();
    newBranch->eraseFromParent();
    newBranch->deleteUseList();
    newHeader->pushInstr(new JumpInst(bodyBlock));

    preValueMap = &newValueMap;
    preClonedLoop = lastCloneLoop;
  }

  // Last loop -> origin loop
  for (auto [oldValue, newValue] : lastCloneLoop->valueMap) {
    if (PhiInst* oldPhi = dynamic_cast<PhiInst*>(oldValue)) {
      PhiInst* newPhi = static_cast<PhiInst*>(newValue);
      if (oldPhi->getParent() != loopInfo->header) continue;
      int icSize = oldPhi->getRValueSize() / 2;
      for (int i = icSize - 1; i >= 0; i--) {
        BasicBlock* icBlock = (BasicBlock*)oldPhi->getRValue(i * 2 + 1);
        if (!loopBlocks.count(icBlock)) continue;
        oldPhi->replaceRValueAt(
            i * 2, lastCloneLoop->getNewValue(oldPhi->getRValue(i * 2)));
        oldPhi->replaceRValueAt(i * 2 + 1, lastCloneLoop->getNewValue(
                                               oldPhi->getRValue(i * 2 + 1)));
      }
    } else if (BranchInst* newBranch = dynamic_cast<BranchInst*>(newValue)) {
      newBranch->replaceDestinationWith(firstCloneLoop->getHeader(),
                                        loopInfo->header);
    } else if (JumpInst* newJump = dynamic_cast<JumpInst*>(newValue)) {
      newJump->replaceDestinationWith(firstCloneLoop->getHeader(),
                                      loopInfo->header);
    }
  }

  // Modify cond expr
  if (endModify) {
    endModify->moveBefore(loopInfo->preHeader->getTailInstr());
    condInstr->replaceRValueAt(1, endModify);
  }

  return changed;
}

uint32_t LoopUnroll::collectBlocksAndCountLine(
    LoopInfo* loopInfo, unordered_set<BasicBlock*>& blocks) {
  uint32_t size = 0;
  for (BasicBlock* block : loopInfo->blocks) {
    blocks.insert(block);
    size += block->getInstructions()->getSize();
  }
  for (LoopInfo* subloop : loopInfo->subLoops) {
    size += collectBlocksAndCountLine(subloop, blocks);
  }
  return size;
}

bool LoopUnroll::constantOrInvariant(Value* value, LoopInfo* loopInfo) {
  if (dynamic_cast<Constant*>(value) || dynamic_cast<Argument*>(value))
    return true;
  Instruction* instr = dynamic_cast<Instruction*>(value);
  if (!instr) return false;
  if (loopInfo->containBlockInChildren(instr->getParent())) return false;
  return true;
}
