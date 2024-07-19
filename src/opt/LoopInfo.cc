#include "LoopInfo.hh"

LoopInfo* LoopInfo::getRootLoop() {
  LoopInfo* ptr = this;
  while (ptr->parentLoop) {
    ptr = ptr->parentLoop;
  }
  return ptr;
}

bool LoopInfo::containBlockInChildren(BasicBlock* block) {
  if (containBlock(block)) return true;

  for (LoopInfo* subloop : subLoops) {
    if (subloop->containBlockInChildren(block)) return true;
  }
  return false;
}

void LoopInfo::addBlock(BasicBlock* block) {
  if (liBase) {
    liBase->addBlockToLoop(block, this);
  } else {
    blocks.insert(block);
  }
}

void LoopInfo::addSubLoop(LoopInfo* subloop) {
  subLoops.push_back(subloop);
  subloop->setParentLoop(this);
}

void LoopInfo::setLatches(vector<BasicBlock*>& latches_) {
  for (BasicBlock* latch : latches_) {
    latches.insert(latch);
  }
}

void LoopInfo::dump() {
  std::cout << "Header: " << header->getName() << std::endl;
  std::cout << "Parent: " << (parentLoop ? parentLoop->getName() : "Null")
            << std::endl;
  std::cout << "SubLoop:";
  for (LoopInfo* subLoop : subLoops) {
    std::cout << subLoop->getName() << " ";
  }
  std::cout << std::endl;
  std::cout << "Blocks:";
  for (BasicBlock* block : blocks) {
    std::cout << block->getName() << " ";
  }
  std::cout << std::endl;
  std::cout << std::endl << std::endl;
}

LoopInfo* LoopInfoBase::getLoopOf(BasicBlock* block) {
  auto it = bbToLoop.find(block);
  if (it == bbToLoop.end()) {
    return nullptr;
  }
  return it->second;
}

void LoopInfoBase::addLoopInfo(LoopInfo* loopInfo) {
  loopInfos.push_back(loopInfo);
  for (BasicBlock* block : loopInfo->blocks) {
    bbToLoop[block] = loopInfo;
  }
}

void LoopInfoBase::addBlockToLoop(BasicBlock* block, LoopInfo* loopInfo) {
  loopInfo->blocks.insert(block);
  bbToLoop[block] = loopInfo;
}

void LoopInfoBase::calculateDepth() {
  std::function<void(LoopInfo*, uint32_t)> dfs = [&](LoopInfo* loopInfo,
                                                     uint32_t depth) {
    loopInfo->depth = depth;
    for (LoopInfo* subloop : loopInfo->subLoops) {
      dfs(subloop, depth + 1);
    }
  };

  for (LoopInfo* loopInfo : loopInfos) {
    if (!loopInfo->parentLoop) {
      dfs(loopInfo, 1);
    }
  }
}

uint32_t LoopInfoBase::getDepth(BasicBlock* block) {
  LoopInfo* loopInfo = getLoopOf(block);
  if (!loopInfo) return 0;
  return loopInfo->depth;
}

void LoopInfoBase::dump() {
  for (LoopInfo* loop : loopInfos) {
    loop->dump();
  }
}

void LoopInfo::analyseSimpleLoop() {
  // not a simple loop
  if (!(exits.size() == 1 && exitings.size() == 1 && latches.size() == 1 &&
        *exitings.begin() == header))
    return;
  BranchInst* branchInst = dynamic_cast<BranchInst*>(header->getTailInstr());
  if (!branchInst) return;
  Value* condValue = branchInst->getRValue(0);
  Instruction* condInstr = dynamic_cast<Instruction*>(condValue);
  if (!condInstr) return;
  PhiInst* phiInst;
  Value* endValue;
  if (condInstr->isa(VT_ICMP) || condInstr->isa(VT_FCMP)) {
    Value* lhs = condInstr->getRValue(0);
    Value* rhs = condInstr->getRValue(1);
    if (lhs->isa(VT_PHI) && ((Instruction*)lhs)->getParent() == header) {
      phiInst = (PhiInst*)lhs;
      endValue = rhs;
    } else if (rhs->isa(VT_PHI) && ((Instruction*)rhs)->getParent() == header) {
      phiInst = (PhiInst*)rhs;
      endValue = lhs;
      condInstr->swapRValueAt(0, 1);
      if (IcmpInst* icmpInst = dynamic_cast<IcmpInst*>(condInstr)) {
        if (icmpInst->getOpTag() == SLE) {
          icmpInst->setOpTag(SGE);
        } else if (icmpInst->getOpTag() == SLT) {
          icmpInst->setOpTag(SGT);
        } else if (icmpInst->getOpTag() == SGE) {
          icmpInst->setOpTag(SLE);
        } else if (icmpInst->getOpTag() == SGT) {
          icmpInst->setOpTag(SLT);
        }
      } else if (FcmpInst* fcmpInst = dynamic_cast<FcmpInst*>(condInstr)) {
        if (fcmpInst->getOpTag() == OLE) {
          fcmpInst->setOpTag(OGE);
        } else if (fcmpInst->getOpTag() == OLT) {
          fcmpInst->setOpTag(OGT);
        } else if (fcmpInst->getOpTag() == OGE) {
          fcmpInst->setOpTag(OLE);
        } else if (fcmpInst->getOpTag() == OGT) {
          fcmpInst->setOpTag(OLT);
        }
      }
    } else {
      return;
    }
  } else {
    return;
  }

  // find init and stride
  if (phiInst->getRValueSize() != 4) return;
  Value* initValue;
  BinaryOpInst* strideInstr;
  if ((BasicBlock*)phiInst->getRValue(1) == *latches.begin()) {
    strideInstr = dynamic_cast<BinaryOpInst*>(phiInst->getRValue(0));
    initValue = phiInst->getRValue(2);
  } else if ((BasicBlock*)phiInst->getRValue(3) == *latches.begin()) {
    strideInstr = dynamic_cast<BinaryOpInst*>(phiInst->getRValue(2));
    initValue = phiInst->getRValue(0);
  } else {
    return;
  }
  if (!strideInstr) return;

  // adjust stride step: i = 1 + i=> i = i + 1
  if (strideInstr->getOpTag() == ADD || strideInstr->getOpTag() == FADD ||
      strideInstr->getOpTag() == MUL || strideInstr->getOpTag() == FMUL) {
    if (strideInstr->getRValue(0) == phiInst) {
    } else if (strideInstr->getRValue(1) == phiInst) {
      strideInstr->swapRValueAt(0, 1);
    } else {
      return;
    }
  } else if (strideInstr->getOpTag() == SUB ||
             strideInstr->getOpTag() == FSUB) {
    if (strideInstr->getRValue(0) != phiInst) return;
  } else {
    return;
  }

  if (PhiInst* phiInst = dynamic_cast<PhiInst*>(initValue)) {
    if (phiInst->getRValueSize() == 2) {
      initValue = phiInst->getRValue(0);
      phiInst->replaceAllUsesWith(initValue);
      phiInst->eraseFromParent();
      phiInst->deleteUseList();
    }
  }

  // // Check whether it'a pure induction variable
  // // (not used by other instruction in loop)
  // bool pureIdv = true;
  // for (Use* use = strideInstr->getUseHead(); use; use = use->next) {
  //   Instruction* userInstr = use->instr;
  //   if (userInstr != phiInst) {
  //     pureIdv = false;
  //     break;
  //   }
  // }
  // for (Use* use = phiInst->getUseHead(); use; use = use->next) {
  //   Instruction* userInstr = use->instr;
  //   if (userInstr != strideInstr &&
  //       containBlockInChildren(userInstr->getParent())) {
  //     pureIdv = false;
  //     break;
  //   }
  // }

  simpleLoop = new SimpleLoopInfo();
  simpleLoop->brInstr = branchInst;
  simpleLoop->phiInstr = phiInst;
  simpleLoop->strideInstr = strideInstr;
  simpleLoop->initValue = initValue;
  // simpleLoop->pureIdv = pureIdv;
}

void LoopInfoBase::analyseSimpleLoop() {
  for (LoopInfo* loopInfo : loopInfos) {
    loopInfo->analyseSimpleLoop();
  }
}
