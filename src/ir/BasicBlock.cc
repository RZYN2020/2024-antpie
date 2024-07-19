#include "BasicBlock.hh"

#include "Function.hh"

BasicBlock::~BasicBlock() {
  for (const auto& instr : instructions) {
    delete instr;
  }
}

void BasicBlock::printIR(ostream& stream) const {
  stream << getName() << ":" << endl;
  for (const auto& instr : instructions) {
    stream << "  ";
    instr->printIR(stream);
    stream << endl;
  }
}

bool isTail(Instruction* instr) {
  return instr->getValueTag() == VT_BR || instr->getValueTag() == VT_RET ||
         instr->getValueTag() == VT_JUMP;
}

void BasicBlock::pushInstr(Instruction* instr) {
  instr->setParent(this);
  instructions.pushBack(instr);
  if (isTail(instr)) {
    if (tail) {
      // std::cout << "Multiple jump instruction in bb";
    }
    tail = instr;
  }
}

void BasicBlock::pushInstrAtHead(Instruction* instr) {
  instr->setParent(this);
  instructions.pushFront(instr);
}

void BasicBlock::eraseFromParent() {
  getParent()->getBasicBlocks()->remove(this);
  function = nullptr;
}

BasicBlock* BasicBlock::clone(unordered_map<Value*, Value*>& replaceMap) {
  BasicBlock* newBlock = new BasicBlock(LabelManager::getLabel(getName()));
  for (Instruction* instr : instructions) {
    Instruction* newInstr = instr->clone();
    newInstr->setName(LabelManager::getLabel(newInstr->getName()));
    newBlock->pushInstr(newInstr);
    replaceMap.emplace(instr, newInstr);
  }
  return newBlock;
}

BasicBlock* BasicBlock::split(LinkedList<Instruction*>::Iterator iter) {
  BasicBlock* splitBlock = new BasicBlock(getName() + "_split");
  instructions.splitAfter(iter, splitBlock->getInstructions());
  splitBlock->tail = tail;
  tail = nullptr;
  splitBlock->function = function;
  return splitBlock;
}

// This method introduces at least one new basic block into the function and
// moves some of the predecessors of BB to be predecessors of the new block.
BasicBlock* BasicBlock::splitBlockPredecessors(vector<BasicBlock*>& preds) {
  BasicBlock* newBlock = new BasicBlock(getName() + "_pred");
  JumpInst* jumpInst = new JumpInst(this);
  newBlock->pushInstr(jumpInst);
  function->insertBasicBlockBefore(newBlock, this);

  CFG* cfg = function->getCFG();
  if (cfg) {
    cfg->addNode(newBlock);
    cfg->addEdge(newBlock, this);
  }

  for (BasicBlock* pred : preds) {
    Instruction* tailInstr = pred->getTailInstr();
    if (!tailInstr) {
      JumpInst* jump = new JumpInst(newBlock);
      pred->pushInstr(jump);
      if (cfg) {
        cfg->addEdge(pred, newBlock);
      }
    } else if (tailInstr->isa(VT_JUMP)) {
      JumpInst* jump = dynamic_cast<JumpInst*>(tailInstr);
      jump->replaceDestinationWith(this, newBlock);
    } else if (tailInstr->isa(VT_BR)) {
      BranchInst* branch = dynamic_cast<BranchInst*>(tailInstr);
      branch->replaceDestinationWith(this, newBlock);
    }
  }

  // Modify PHI
  for (Instruction* instr : *getInstructions()) {
    PhiInst* phiInstr;
    if (phiInstr = dynamic_cast<PhiInst*>(instr)) {
      PhiInst* newPhi = new PhiInst(phiInstr->getName() + "pre_clone");
      for (BasicBlock* pred : preds) {
        Value* fromValue = phiInstr->deleteIncomingFrom(pred);
        newPhi->pushIncoming(fromValue, pred);
      }
      phiInstr->pushIncoming(newPhi, newBlock);
      newBlock->pushInstrAtHead(newPhi);
    } else {
      break;
    }
  }

  return newBlock;
}