#include "CSE.hh"

string CommonSubexpElimination::hashToString(Instruction* instr) {
  assert(isSimpleExpr(instr));
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
  }

  if (instr->getRValueSize() == 1) {
    return opName + "|" + lhs->toString();
  } else {
    return opName + "|" + lhs->toString() + "|" + rhs->toString();
  }
}

bool CommonSubexpElimination::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  for (Function* func : *module->getFunctions()) {
    changed |= runOnFunction(func);
  }
  return changed;
}

bool CommonSubexpElimination::runOnFunction(Function* func) {
  if (!func->getCFG()) func->buildCFG();
  if (!func->getDT()) func->buildDT();
  domTree = func->getDT();
  trashList.clear();
  currNode = nullptr;
  return cseDfs(func->getEntry());
}

// TODO: Call
bool CommonSubexpElimination::isSimpleExpr(Instruction* instr) {
  return instr->isa(VT_ICMP) || instr->isa(VT_FCMP) || instr->isa(VT_BOP) ||
         instr->isa(VT_FPTOSI) || instr->isa(VT_SITOFP) ||
         instr->isa(VT_ZEXT) || instr->isa(VT_GEP);
}

bool CommonSubexpElimination::cseDfs(BasicBlock* block) {
  bool changed = false;
  pushNode();
  for (Instruction* instr : *block->getInstructions()) {
    // Expr can not be optimize
    if (!isSimpleExpr(instr)) continue;

    string hashStr = hashToString(instr);
    Instruction* comExpr = nullptr;
    if ((comExpr = currNode->findInstruction(hashStr))) {
      instr->replaceAllUsesWith(comExpr);
      trashList.pushBack(instr);
      changed = true;
    } else {
      currNode->pushInstruction(hashStr, instr);
    }
  }

  // remove unuse instruction
  while (!trashList.isEmpty()) {
    Instruction* trash = trashList.popFront();
    trash->eraseFromParent();
    trash->deleteUseList();
    delete trash;
  }

  // dfs via domTree
  for (BasicBlock* succ : *domTree->getDomChildren(block)) {
    changed |= cseDfs(succ);
  }

  popNode();

  return changed;
}
