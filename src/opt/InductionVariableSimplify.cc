#include "InductionVariableSimplify.hh"

#include "ConstantFolding.hh"
bool InductionVariableSimplify::runOnModule(ANTPIE::Module* module) {
  bool changed = false;
  vector<Function*> changedFunc;
  for (Function* function : *module->getFunctions()) {
    bool c = runOnFunction(function);
    changed |= c;
    if (c) changedFunc.push_back(function);
  }
  ConstantFolding cf;
  for (Function* func : changedFunc) {
    cf.runOnFunction(func);
  }
  return changed;
}

bool InductionVariableSimplify::runOnFunction(Function* func) {
  bool changed = false;
  for (LoopInfo* loopInfo : func->getLoopInfoBase()->loopInfos) {
    changed |= runOnLoop(loopInfo);
  }
  if (!changed) return false;

  // delete empty loop
  LinkedList<LoopInfo*> worklist;
  for (LoopInfo* loopInfo : func->getLoopInfoBase()->loopInfos) {
    if (loopInfo->isEmptyLoop()) {
      worklist.pushBack(loopInfo);
    }
  }
  while (!worklist.isEmpty()) {
    LoopInfo* loopInfo = worklist.popFront();
    LoopInfo* parent = loopInfo->parentLoop;
    loopInfo->deleteLoop();
    if (parent && parent->isEmptyLoop()) {
      worklist.pushBack(parent);
    }
  }
  return changed;
}

/**
 * Induction Variable fit this form:
 *
 * Header:
 *  x = phi [0, x1]
 *
 * LoopBody:
 *  x1 = x + a
 *
 * phi is only user of x1
 * x1 is only user of x in loop
 * a is loop invarient variable
 */

bool InductionVariableSimplify::runOnLoop(LoopInfo* loopInfo) {
  // 1. check it is a simplie loop and
  // repeat time can be calculate
  SimpleLoopInfo* simpleLoop = loopInfo->simpleLoop;
  if (!simpleLoop) return false;
  BranchInst* brInst = simpleLoop->brInstr;
  Value* initValue = simpleLoop->initValue;
  BinaryOpInst* strideInstr = simpleLoop->strideInstr;
  Value* stride = strideInstr->getRValue(1);
  Instruction* condInstr = (Instruction*)simpleLoop->brInstr->getRValue(0);
  Value* endValue = condInstr->getRValue(1);
  IcmpInst* icmpCond = dynamic_cast<IcmpInst*>(condInstr);
  if (!icmpCond) return false;
  // check if init, stride is loop invariant
  if (!isInvariant(loopInfo, initValue)) {
    return false;
  }
  if (!dynamic_cast<Constant*>(stride)) {
    return false;
  }
  // check end is loop invariant
  if (!isInvariant(loopInfo, endValue)) return false;

  // 2. find all induction variable
  BasicBlock* header = loopInfo->header;
  vector<IndVar*> indVars;
  for (Instruction* instr : *header->getInstructions()) {
    if (!instr->isa(VT_PHI)) break;
    PhiInst* phiInst = static_cast<PhiInst*>(instr);
    if (phiInst->getRValueSize() != 4) continue;
    Value* initValue = 0;
    Value* strideInstr = 0;
    BinaryOpInst* remBop = 0;
    if (!loopInfo->containBlockInChildren((BasicBlock*)phiInst->getRValue(1))) {
      initValue = phiInst->getRValue(0);
    } else {
      strideInstr = phiInst->getRValue(0);
    }
    if (!loopInfo->containBlockInChildren((BasicBlock*)phiInst->getRValue(3))) {
      initValue = phiInst->getRValue(2);
    } else {
      strideInstr = phiInst->getRValue(2);
    }

    if (!initValue || !strideInstr) continue;
    BinaryOpInst* strideBop = dynamic_cast<BinaryOpInst*>(strideInstr);
    if (!strideBop) continue;
    Value* strideValue = 0;

    if (strideBop->getOpTag() == SREM) {
      remBop = strideBop;
      strideBop = dynamic_cast<BinaryOpInst*>(remBop->getRValue(0));
      if (!strideBop) continue;
      if (strideBop->getRValue(1) != phiInst) continue;
      if (!isInvariant(loopInfo, strideBop->getRValue(0))) continue;
      if (strideBop->getUserSize() != 1 || strideBop->getOpTag() != ADD)
        continue;
      strideValue = strideBop->getRValue(0);
      if (!isInvariant(loopInfo, remBop->getRValue(1))) continue;
      IndVar* indVar = new IndVar();
      *indVar = {.phiInst = phiInst,
                 .strideInst = strideBop,
                 .strideValue = strideValue,
                 .initValue = initValue,
                 .remBop = remBop};
      indVars.push_back(indVar);
      continue;
    }
    if (strideBop->getRValue(0) == phiInst) {
      strideValue = strideBop->getRValue(1);
    } else if (strideBop->getRValue(1) == phiInst) {
      strideValue = strideBop->getRValue(0);
    }

    if (!strideValue) continue;

    // checkout if there are other users
    if (strideBop->getUserSize() != 1) continue;
    bool flag = false;
    for (Use* use = phiInst->getUseHead(); use; use = use->next) {
      Instruction* userInst = use->instr;
      if (loopInfo->containBlockInChildren(userInst->getParent()) &&
          userInst != strideInstr) {
        flag = true;
        break;
      }
    }
    if (flag) continue;

    // checkout stride value is loop invariant
    if (!isInvariant(loopInfo, strideValue)) continue;

    OpTag op = strideBop->getOpTag();
    if (!(op == ADD || op == SUB)) continue;

    IndVar* indVar = new IndVar();
    *indVar = {.phiInst = phiInst,
               .strideInst = strideBop,
               .strideValue = strideValue,
               .initValue = initValue,
               .remBop = remBop};
    indVars.push_back(indVar);
  }
  if (indVars.empty()) return false;

  Value* loopTime = 0;

  OpTag icmpOp = icmpCond->getOpTag();
  switch (strideInstr->getOpTag()) {
    case ADD: {
      if (icmpOp == SLT || icmpOp == SGT || icmpOp == NE) {
        BinaryOpInst* distance =
            new BinaryOpInst(SUB, endValue, initValue, "idv.dist");
        loopTime = new BinaryOpInst(SDIV, distance, stride, "idv.time");
        distance->moveBefore(brInst);
        ((Instruction*)loopTime)->moveBefore(brInst);
      } else if (icmpOp == SLE) {
        BinaryOpInst* distance =
            new BinaryOpInst(SUB, endValue, initValue, "idv.dist");
        distance->moveBefore(brInst);
        distance = new BinaryOpInst(
            ADD, distance, IntegerConstant::getConstInt(1), "idv.dist");
        distance->moveBefore(brInst);
        loopTime = new BinaryOpInst(SDIV, distance, stride, "idv.time");
        ((Instruction*)loopTime)->moveBefore(brInst);
      } else if (icmpOp == SGE) {
        BinaryOpInst* distance =
            new BinaryOpInst(SUB, endValue, initValue, "idv.dist");
        distance->moveBefore(brInst);
        distance = new BinaryOpInst(
            SUB, distance, IntegerConstant::getConstInt(1), "idv.dist");
        distance->moveBefore(brInst);
        loopTime = new BinaryOpInst(SDIV, distance, stride, "idv.time");
        ((Instruction*)loopTime)->moveBefore(brInst);
      } else {
        return false;
      }
      break;
    }
    case SUB: {
      if (icmpOp == SLT || icmpOp == SGT || icmpOp == NE) {
        BinaryOpInst* distance =
            new BinaryOpInst(SUB, initValue, endValue, "idv.dist");
        loopTime = new BinaryOpInst(SDIV, distance, stride, "idv.time");
        distance->moveBefore(brInst);
        ((Instruction*)loopTime)->moveBefore(brInst);
      } else if (icmpOp == SLE) {
        BinaryOpInst* distance =
            new BinaryOpInst(SUB, initValue, endValue, "idv.dist");
        distance->moveBefore(brInst);
        distance = new BinaryOpInst(
            SUB, distance, IntegerConstant::getConstInt(1), "idv.dist");
        distance->moveBefore(brInst);
        loopTime = new BinaryOpInst(SDIV, distance, stride, "idv.time");
        ((Instruction*)loopTime)->moveBefore(brInst);
      } else if (icmpOp == SGE) {
        BinaryOpInst* distance =
            new BinaryOpInst(SUB, initValue, endValue, "idv.dist");
        distance->moveBefore(brInst);
        distance = new BinaryOpInst(
            ADD, distance, IntegerConstant::getConstInt(1), "idv.dist");
        distance->moveBefore(brInst);
        loopTime = new BinaryOpInst(SDIV, distance, stride, "idv.time");
        ((Instruction*)loopTime)->moveBefore(brInst);
      } else {
        return false;
      }
      break;
    }
    default:
      return false;
  }

  // 3. replace induction variable
  for (IndVar* indVar : indVars) {
    Instruction* replaceValue = 0;

    if (indVar->remBop) {
      Value* modValue = indVar->remBop->getRValue(1);
      Value* stride = indVar->strideValue;
      auto module = header->getParent()->getParent();
      Function* multimod = module->getLibFunction("antpie_multimod");
      module->pushExternFunction(multimod);
      vector<Value*> args = {stride, loopTime, modValue};
      CallInst* mmCall = new CallInst(multimod, args, "multimod");
      replaceValue =
          new BinaryOpInst(ADD, mmCall, indVar->initValue, "idv.ret");
      mmCall->moveBefore(brInst);
      indVar->remBop->deleteUseList();
      indVar->remBop->eraseFromParent();
    } else {
      switch (indVar->strideInst->getOpTag()) {
        case ADD: {
          BinaryOpInst* mul =
              new BinaryOpInst(MUL, indVar->strideValue, loopTime, "idv.inc");
          replaceValue =
              new BinaryOpInst(ADD, mul, indVar->initValue, "idv.ret");
          mul->moveBefore(brInst);
          break;
        }
        case SUB: {
          BinaryOpInst* mul =
              new BinaryOpInst(MUL, indVar->strideValue, loopTime, "idv.inc");
          replaceValue =
              new BinaryOpInst(SUB, indVar->initValue, mul, "idv.ret");
          mul->moveBefore(brInst);
          break;
        }
        default:
          assert(0);
      }
    }
    replaceValue->moveBefore(brInst);
    indVar->strideInst->eraseFromParent();
    indVar->strideInst->deleteUseList();
    indVar->phiInst->replaceAllUsesWith(replaceValue);
    indVar->phiInst->eraseFromParent();
    indVar->phiInst->deleteUseList();
  }
  return !indVars.empty();
}

bool InductionVariableSimplify::isInvariant(LoopInfo* loopInfo, Value* value) {
  Instruction* instr = dynamic_cast<Instruction*>(value);
  if (!instr) return true;
  if (!loopInfo->containBlockInChildren(instr->getParent())) return true;
  return false;
}
