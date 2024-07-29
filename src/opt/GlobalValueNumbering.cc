#include "GlobalValueNumbering.hh"

bool GlobalValueNumbering::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  for (Function* function : *module->getFunctions()) {
    changed |= runOnFunction(function);
  }
  return changed;
}

bool GlobalValueNumbering::runOnFunction(Function* func) {
  bool changed = false;
  CFG* cfg = func->getCFG();
  if (!cfg) cfg = func->buildCFG();

  vector<std::pair<BasicBlock*, int>> postOrder;
  unordered_set<BasicBlock*> visited;
  std::function<void(BasicBlock*, int)> postOrderDfs = [&](BasicBlock* block,
                                                           int depth) {
    for (BasicBlock* succ : *cfg->getSuccOf(block)) {
      if (!visited.count(succ)) {
        visited.insert(succ);
        postOrderDfs(succ, depth + block->getInstructions()->getSize());
      }
    }
    postOrder.emplace_back(block, depth);
  };
  postOrderDfs(func->getEntry(), 0);

  unordered_map<string, vector<std::pair<int, Instruction*>>> exprMap;
  for (auto it = postOrder.rbegin(); it != postOrder.rend(); it++) {
    LinkedList<Instruction*>* instrList = it->first->getInstructions();
    int depth = it->second;
    for (auto instrIt = instrList->begin(); instrIt != instrList->end();) {
      depth++;
      Instruction* instr = *instrIt;
      ++instrIt;
      if (!canNumbering(instr)) continue;

      string hashStr = hashToString(instr);
      auto item = exprMap.find(hashStr);
      if (item != exprMap.end()) {
        Instruction* replaceInstr = 0;
        int preDist = INT32_MAX;
        for (auto& [preDepth, preInstr] : item->second) {
          if (preInstr->getParent() == instr->getParent()) {
            replaceInstr = preInstr;
            break;
          }
          int dist = std::abs(preDepth - depth);
          if (dist <= 100 && dist < preDist) {
            preDist = dist;
            replaceInstr = preInstr;
          }
        }
        if (replaceInstr) {
          changed = true;
          instr->replaceAllUsesWith(replaceInstr);
          instr->eraseFromParent();
          instr->deleteUseList();
        } else {
          item->second.emplace_back(depth, instr);
        }
      } else {
        exprMap[hashStr].emplace_back(depth, instr);
      }
    }
  }
  return changed;
}

string GlobalValueNumbering::hashToString(Instruction* instr) {
  assert(canNumbering(instr));
  Value* lhs = 0;
  Value* rhs = 0;
  string opName;

  if (instr->isa(VT_ICMP)) {
    IcmpInst* icmp = dynamic_cast<IcmpInst*>(instr);
    lhs = instr->getRValue(0);
    rhs = instr->getRValue(1);
    OpTag opTag = icmp->getOpTag();

    if (opTag == EQ || opTag == NE) {
      if (lhs < rhs) {
        std::swap(lhs, rhs);
      }
    } else if (opTag == SGE) {
      std::swap(lhs, rhs);
      opTag = SLE;
    } else if (opTag == SGT) {
      std::swap(lhs, rhs);
      opTag = SLT;
    }
    opName = instr->getOpName(opTag);

  } else if (instr->isa(VT_FCMP)) {
    FcmpInst* fcmp = dynamic_cast<FcmpInst*>(instr);
    lhs = instr->getRValue(0);
    rhs = instr->getRValue(1);
    OpTag opTag = fcmp->getOpTag();

    if (opTag == OEQ || opTag == ONE) {
      if (lhs < rhs) {
        std::swap(lhs, rhs);
      }
    } else if (opTag == OGE) {
      std::swap(lhs, rhs);
      opTag = OLE;
    } else if (opTag == OGT) {
      std::swap(lhs, rhs);
      opTag = OLT;
    }
    opName = instr->getOpName(opTag);
  } else if (instr->isa(VT_BOP)) {
    BinaryOpInst* bop = dynamic_cast<BinaryOpInst*>(instr);
    lhs = instr->getRValue(0);
    rhs = instr->getRValue(1);
    OpTag opTag = bop->getOpTag();
    if (opTag == ADD || opTag == FADD || opTag == MUL || opTag == FMUL ||
        opTag == AND || opTag == OR || opTag == XOR) {
      if (lhs < rhs) {
        std::swap(lhs, rhs);
      }
    }
    opName = instr->getOpName(opTag);
  } else if (instr->isa(VT_FPTOSI)) {
    FptosiInst* fptosi = dynamic_cast<FptosiInst*>(instr);
    lhs = instr->getRValue(0);
    opName = "fptosi";
  } else if (instr->isa(VT_SITOFP)) {
    SitofpInst* sitofp = dynamic_cast<SitofpInst*>(instr);
    lhs = instr->getRValue(0);
    opName = "sitofp";
  } else if (instr->isa(VT_ZEXT)) {
    ZextInst* zext = dynamic_cast<ZextInst*>(instr);
    lhs = instr->getRValue(0);
    opName = "zext" + zext->getType()->toString();
  } else if (instr->isa(VT_GEP)) {
    GetElemPtrInst* gep = dynamic_cast<GetElemPtrInst*>(instr);
    lhs = instr->getRValue(1);

    if (instr->getRValueSize() == 3) {
      rhs = instr->getRValue(2);
    }
    opName = "gep" + instr->getRValue(0)->toString();
  } else if (instr->isa(VT_CALL)) {
    CallInst* callInstr = dynamic_cast<CallInst*>(instr);
    string hashStr = "call " + callInstr->getFunction()->getName();
    int argSize = callInstr->getRValueSize();
    for (int i = 0; i < argSize; i++) {
      hashStr += callInstr->getRValue(i)->toString();
    }
    return hashStr;
  }

  if (!rhs) {
    return opName + "|" + lhs->toString();
  } else {
    return opName + "|" + lhs->toString() + "|" + rhs->toString();
  }
}

bool GlobalValueNumbering::canNumbering(Instruction* instr) {
  if (CallInst* callInstr = dynamic_cast<CallInst*>(instr)) {
    Function* callee = callInstr->getFunction();
    return callee->isPureFunction();
  }
  return instr->isa(VT_ICMP) || instr->isa(VT_FCMP) || instr->isa(VT_BOP) ||
         instr->isa(VT_FPTOSI) || instr->isa(VT_SITOFP) ||
         instr->isa(VT_ZEXT) || instr->isa(VT_GEP);
}