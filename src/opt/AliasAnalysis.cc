#include "AliasAnalysis.hh"

#include "AliasAnalysisResult.hh"

bool AliasAnalysis::runOnModule(ANTPIE::Module* module) {
  for (Function* func : *module->getFunctions()) {
    runOnFunction(func);
  }
  return true;
}

bool AliasAnalysis::runOnFunction(Function* func) {
  AliasAnalysisResult* aaResult = new AliasAnalysisResult();
  attr_t attrId = 0;

  attr_t globalAttr = attrId++;
  attr_t stackAttr = attrId++;
  attr_t argAttr = attrId++;

  aaResult->addDistinctPair(globalAttr, stackAttr);
  aaResult->addDistinctPair(stackAttr, argAttr);

  // global variable
  unordered_set<attr_t> globalSet;
  for (GlobalVariable* gv : *func->getParent()->getGlobalVariables()) {
    attr_t gvAttr = attrId++;
    aaResult->pushAttr(gv, globalAttr);
    aaResult->pushAttr(gv, gvAttr);
    globalSet.insert(gvAttr);
  }
  aaResult->addDistinctSet(std::move(globalSet));

  // function args
  FuncType* funcType = dynamic_cast<FuncType*>(func->getType());
  int argSize = funcType->getArgSize();
  for (int i = 0; i < argSize; i++) {
    Argument* arg = funcType->getArgument(i);
    if (arg->isPointer()) {
      aaResult->pushAttr(arg, argAttr);
    }
  }

  // instruction
  unordered_set<attr_t> stackSet;
  for (BasicBlock* block : *func->getBasicBlocks()) {
    for (Instruction* instr : *block->getInstructions()) {
      if (!instr->isPointer()) continue;
      switch (instr->getValueTag()) {
        case VT_ALLOCA: {
          attr_t allocAttr = attrId++;
          stackSet.insert(allocAttr);
          aaResult->pushAttr(instr, allocAttr);
          aaResult->pushAttr(instr, stackAttr);
          break;
        }
        case VT_GEP: {
          Value* base = instr->getRValue(0);
          aaResult->pushAttrs(instr, aaResult->getAttrs(base));
          break;
        }
        default:
          // Unhandled case
          assert(0);
          break;
      }
    }
  }
  aaResult->addDistinctSet(std::move(stackSet));

  func->setAliasAnalysisResult(aaResult);
  return true;
}

/********************* AliasAnalysisResult ***************************/

uint64_t AliasAnalysisResult::hash(attr_t attr1, attr_t attr2) {
  if (attr1 < attr2) {
    std::swap(attr1, attr2);
  }
  return ((uint64_t)attr1 << 32) | attr2;
}

void AliasAnalysisResult::addDistinctPair(attr_t attr1, attr_t attr2) {
  distinctPairs.insert(hash(attr1, attr2));
}

void AliasAnalysisResult::addDistinctSet(unordered_set<attr_t> distinctSet) {
  distinctSets.push_back(distinctSet);
}

bool AliasAnalysisResult::isDistinct(Value* v1, Value* v2) {
  if (v1 == v2 || valueAttrsMap.count(v1) == 0 || valueAttrsMap.count(v2) == 0)
    return false;

  auto& attrSet1 = getAttrs(v1);
  auto& attrSet2 = getAttrs(v2);
  for (attr_t attr1 : attrSet1) {
    for (attr_t attr2 : attrSet2) {
      if (distinctPairs.count(hash(attr1, attr2))) {
        return true;
      }
      if (attr1 == attr2) continue;
      for (auto& distinctSet : distinctSets) {
        if (distinctSet.count(attr1) && distinctSet.count(attr2)) {
          return true;
        }
      }
    }
  }
  return false;
}

void AliasAnalysisResult::pushAttrs(Value* value, vector<attr_t>& attrs) {
  auto& attrsVec = valueAttrsMap[value];
  for (attr_t attr : attrs) {
    attrsVec.push_back(attr);
  }
}

void AliasAnalysisResult::pushAttr(Value* value, attr_t attr) {
  valueAttrsMap[value].push_back(attr);
}

vector<attr_t>& AliasAnalysisResult::getAttrs(Value* value) {
  return valueAttrsMap[value];
}