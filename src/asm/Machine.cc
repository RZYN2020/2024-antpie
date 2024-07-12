#include "Machine.hh"

/////////////////////////////////////////////////
//
//                MachineBasicBlocks
//
/////////////////////////////////////////////////
MBasicBlock::MBasicBlock(string name) {
  this->name = name;
  this->instructions = make_unique<vector<unique_ptr<MInstruction>>>();
  this->jmps = make_unique<vector<unique_ptr<MInstruction>>>();
  this->phis = make_unique<vector<unique_ptr<MHIphi>>>();
  this->incoming = make_unique<vector<MBasicBlock *>>();
  this->outgoing = make_unique<vector<MBasicBlock *>>();
}

void MBasicBlock::pushInstr(MInstruction *i) {
  instructions->push_back(unique_ptr<MInstruction>(i));
  i->setBasicBlock(this);
}

void MBasicBlock::pushInstrs(vector<MInstruction *> is) {
  for (auto i : is) {
    instructions->push_back(unique_ptr<MInstruction>(i));
    i->setBasicBlock(this);
  }
}

void MBasicBlock::pushInstrAtHead(MInstruction *i) {
  instructions->insert(instructions->begin(), unique_ptr<MInstruction>(i));
  i->setBasicBlock(this);
}

void MBasicBlock::pushInstrsAtHead(vector<MInstruction *> is) {
  for (int i = is.size() - 1; i >= 0; i--) {
    pushInstrAtHead(is.at(i));
  }
}

void MBasicBlock::setFunction(MFunction *function) {
  this->function = function;
}

MFunction *MBasicBlock::getFunction() { return function; }

void MBasicBlock::pushJmp(MInstruction *ins) {
  ins->setBasicBlock(this);
  jmps->push_back(unique_ptr<MInstruction>(ins));
  switch (ins->getInsTag()) {
  case MInstruction::H_BR: {
    auto br = static_cast<MHIbr *>(ins);
    outgoing->push_back(br->getFBlock());
    outgoing->push_back(br->getTBlock());
    br->getFBlock()->addIncoming(this);
    br->getTBlock()->addIncoming(this);
    break;
  }
  case MInstruction::BEQ: {
    auto beq = static_cast<MIbeq *>(ins);
    outgoing->push_back(beq->getTargetBB());
    beq->getTargetBB()->addIncoming(this);
    break;
  }
  case MInstruction::BNE: {
    auto bne = static_cast<MIbne *>(ins);
    outgoing->push_back(bne->getTargetBB());
    bne->getTargetBB()->addIncoming(this);
    break;
  }
  case MInstruction::BGE: {
    auto bge = static_cast<MIbge *>(ins);
    outgoing->push_back(bge->getTargetBB());
    bge->getTargetBB()->addIncoming(this);
    break;
  }
  case MInstruction::BLT: {
    auto blt = static_cast<MIblt *>(ins);
    outgoing->push_back(blt->getTargetBB());
    blt->getTargetBB()->addIncoming(this);
    break;
  }
  case MInstruction::J: {
    auto j = static_cast<MIj *>(ins);
    outgoing->push_back(j->getTargetBB());
    j->getTargetBB()->addIncoming(this);
    break;
  }
  case MInstruction::RET:
  case MInstruction::H_RET:
    break;
  default: {
    std::cout << ins->getInsTag() << endl;
    std::cout << *ins << endl;
    assert(0);
  }
  }
}

int MBasicBlock::getJmpNum() { return jmps->size(); }

MInstruction *MBasicBlock::getJmp(int idx) { return &*jmps->at(idx); }

void MBasicBlock::clearJmps() {
  jmps->clear();
  for (auto &bb : *outgoing) {
    bb->removeIncoming(this);
  }
  outgoing->clear();
}

void MBasicBlock::pushPhi(MHIphi *phi) {
  phis->push_back(std::unique_ptr<MHIphi>(phi));
}

void MBasicBlock::removeIncoming(MBasicBlock *bb) {
  auto it = incoming->begin();
  while (it != incoming->end()) {
    if (*it == bb) {
      it = incoming->erase(it);
    } else {
      ++it;
    }
  }
}
void MBasicBlock::addIncoming(MBasicBlock *bb) { incoming->push_back(bb); }

// control the incoming/out coming relation
void MBasicBlock::replaceOutgoing(MBasicBlock *oldbb, MBasicBlock *newbb) {
  for (auto it = outgoing->begin(); it != outgoing->end(); ++it) {
    if (*it == oldbb) {
      *it = newbb;
      break;
    }
  }
  oldbb->removeIncoming(this);
  newbb->addIncoming(this);
  for (auto &jmp : *jmps) {
    auto ins = &*jmp;
    switch (ins->getInsTag()) {
    case MInstruction::H_BR: {
      auto br = static_cast<MHIbr *>(ins);
      if (br->getFBlock() == oldbb) {
        br->setFBlock(newbb);
      }
      if (br->getTBlock() == oldbb) {
        br->setTBlock(newbb);
      }
      break;
    }
    case MInstruction::BEQ: {
      auto i = static_cast<MIbeq *>(ins);
      if (i->getTargetBB() == oldbb) {
        i->setTargetBB(newbb);
      }
      break;
    }
    case MInstruction::BNE: {
      auto i = static_cast<MIbne *>(ins);
      if (i->getTargetBB() == oldbb) {
        i->setTargetBB(newbb);
      }
      break;
    }
    case MInstruction::BGE: {
      auto i = static_cast<MIbge *>(ins);
      if (i->getTargetBB() == oldbb) {
        i->setTargetBB(newbb);
      }
      break;
    }
    case MInstruction::BLT: {
      auto i = static_cast<MIblt *>(ins);
      if (i->getTargetBB() == oldbb) {
        i->setTargetBB(newbb);
      }
      break;
    }
    case MInstruction::J: {
      auto i = static_cast<MIj *>(ins);
      if (i->getTargetBB() == oldbb) {
        i->setTargetBB(newbb);
      }
      break;
    }
    case MInstruction::RET:
    case MInstruction::H_RET:
      break;
    default:
      assert(0);
    }
  }
}

// only affect phi
void MBasicBlock::replacePhiIncoming(MBasicBlock *oldbb, MBasicBlock *newbb) {
  for (auto &phi : *phis) {
    phi->replaceIncoming(oldbb, newbb);
  }
}
vector<MBasicBlock *> &MBasicBlock::getIncomings() { return *incoming; }
vector<MBasicBlock *> &MBasicBlock::getOutgoings() { return *outgoing; }

unique_ptr<MInstruction> MBasicBlock::removeInstruction(MInstruction *ins) {
  for (auto it = instructions->begin(); it != instructions->end();) {
    if (it->get() == ins) {
      unique_ptr<MInstruction> removed = std::move(*it);
      it = instructions->erase(it);
      removed->setBasicBlock(nullptr);
      return removed;
    } else {
      ++it;
    }
  }
  return nullptr;
}

void MBasicBlock::replaceInstructionWith(MInstruction *ins,
                                         vector<MInstruction *> instrs) {

  for (auto it = jmps->begin(); it != jmps->end(); ++it) {
    if (it->get() == ins) {
      ins->setBasicBlock(nullptr);
      jmps->erase(it);

      for (auto new_ins : instrs) {
        jmps->insert(it, unique_ptr<MInstruction>(new_ins));
        new_ins->setBasicBlock(this);
        ++it;
      }
      return;
    }
  }

  for (auto it = instructions->begin(); it != instructions->end(); ++it) {
    if (it->get() == ins) {
      ins->setBasicBlock(nullptr);
      instructions->erase(it);

      for (auto new_ins : instrs) {
        instructions->insert(it, unique_ptr<MInstruction>(new_ins));
        new_ins->setBasicBlock(this);
        ++it;
      }
      return;
    }
  }
}

void MBasicBlock::insertBeforeInstructionWith(MInstruction *ins,
                                              vector<MInstruction *> instrs) {
  for (auto it = jmps->begin(); it != jmps->end(); ++it) {
    if (it->get() == ins) {
      for (auto new_ins : instrs) {
        instructions->push_back(unique_ptr<MInstruction>(new_ins));
        new_ins->setBasicBlock(this);
      }
      return;
    }
  }

  for (auto it = instructions->begin(); it != instructions->end(); ++it) {
    if (it->get() == ins) {
      for (auto new_ins : instrs) {
        it = instructions->insert(it, unique_ptr<MInstruction>(new_ins));
        new_ins->setBasicBlock(this);
        ++it;
      }
      return;
    }
  }
}

void MBasicBlock::insertAfterInstructionWith(MInstruction *ins,
                                             vector<MInstruction *> instrs) {
  for (auto it = instructions->begin(); it != instructions->end(); ++it) {
    if (it->get() == ins) {
      ++it;
      for (auto new_ins : instrs) {
        it = instructions->insert(it, unique_ptr<MInstruction>(new_ins));
        new_ins->setBasicBlock(this);
        ++it;
      }
      return;
    }
  }
}

vector<unique_ptr<MHIphi>> &MBasicBlock::getPhis() { return *phis; }

vector<unique_ptr<MInstruction>> &MBasicBlock::getInstructions() {
  return *instructions;
}

vector<MInstruction *> MBasicBlock::getAllInstructions() {
  vector<MInstruction *> res = {};
  for (auto &ins : *instructions) {
    res.push_back(ins.get());
  }
  for (auto &ins : *jmps) {
    res.push_back(ins.get());
  }
  return res;
}

vector<unique_ptr<MInstruction>> &MBasicBlock::getJmps() { return *jmps; }

std::ostream &operator<<(std::ostream &os, const MBasicBlock &obj) {
  os << obj.getName() << ":" << endl;
  auto mod = obj.function->getMod();
  if (mod->is_ssa()) {
    for (auto &phi : *obj.phis) {
      auto com = phi->getComment();
      if (com == "") {
        os << "\t" << *phi << endl;
      } else {
        os << "\t" << *phi << " #" << com << endl;
      }
    }
  }
  for (auto &ins : *obj.instructions) {
    auto com = ins->getComment();
    if (com == "") {
      os << "\t" << *ins << endl;
    } else {
      os << "\t" << *ins << " #" << com << endl;
    }
  }

  for (auto &jmp : *obj.jmps) {
    auto com = jmp->getComment();
    if (com == "") {
      os << "\t" << *jmp << endl;
    } else {
      os << "\t" << *jmp << " #" << com << endl;
    }
  }
  return os;
}

/////////////////////////////////////////////////
//
//                MachineGlobal
//
/////////////////////////////////////////////////

union FloatIntUnion {
  float f;
  int32_t i;
};

int32_t float_to_int_bits(float f) {
  FloatIntUnion u;
  u.f = f;
  return u.i;
}

string MGlobal::getName() const { return global->getName(); }

static void add_decl(std::ostream &os, Constant *init, Type *tp) {
  switch (tp->getTypeTag()) {
  case TT_INT32: {
    os << "\t.word " << init->toString() << endl;
    break;
  }
  case TT_INT1:
    os << "\t.word " << init->toString() << endl;
    break;
  case TT_FLOAT: {
    auto f = static_cast<FloatConstant *>(init);
    os << "\t.word " + std::to_string(float_to_int_bits(f->getValue())) << endl;
    break;
  }
  case TT_ARRAY: {
    ArrayType *arrType = static_cast<ArrayType *>(tp);
    Type *elemType = arrType->getElemType();
    int size = arrType->getLen();
    if (init->isZeroInit()) {
      os << "\t.zero " << std::to_string(size * cal_size(elemType)) << endl;
      break;
    }
    auto arrInit = static_cast<ArrayConstant *>(init);
    for (int i = 0; i < size; i++) {
      Constant *elemInit = arrInit->getElemInit(i);
      add_decl(os, elemInit, elemType);
    }
    break;
  }
  case TT_POINTER: {
    PointerType *p = static_cast<PointerType *>(tp);
    Type *elemType = p->getElemType();
    add_decl(os, init, elemType);
    break;
  }
  default: {
    std::cout << tp->getTypeTag() << endl;
    assert(0);
  }
  }
}

std::ostream &operator<<(std::ostream &os, const MGlobal &obj) {
  os << obj.getName() << ":" << endl;
  auto tp = obj.global->getType();
  auto init = obj.global->getInitValue();
  add_decl(os, init, tp);
  return os;
}

/////////////////////////////////////////////////
//
//                MachineFunction
//
/////////////////////////////////////////////////

MFunction::MFunction(FuncType *type, string name) {
  this->type = type;
  this->name = name;
  this->basicBlocks = make_unique<vector<unique_ptr<MBasicBlock>>>();
  this->parameters = make_unique<vector<unique_ptr<ParaRegister>>>();
  int float_cnt = 10;
  int int_cnt = 10;
  int offset = 0;
  for (int i = 0; i < type->getArgSize(); i++) {
    auto arg = type->getArgument(i);
    auto tp = arg->getType();
    ParaRegister *argr;
    switch (tp->getTypeTag()) {
    case TT_INT1:
    case TT_INT32: {
      if (int_cnt <= 17) {
        argr = new ParaRegister(Register::getIRegister(int_cnt++),
                                arg->getName(), Register::V_IREGISTER, false);
      } else {
        argr = new ParaRegister(offset, 4, arg->getName(),
                                Register::V_IREGISTER, false);
        offset += 4;
      }
      break;
    }
    case TT_FLOAT: {
      if (int_cnt <= 17) {
        argr = new ParaRegister(Register::getFRegister(int_cnt++),
                                arg->getName(), Register::V_FREGISTER, false);
      } else {
        argr = new ParaRegister(offset, 4, arg->getName(),
                                Register::V_FREGISTER, false);
        offset += 4;
      }
      break;
    }
    case TT_POINTER: {
      if (int_cnt <= 17) {
        argr = new ParaRegister(Register::getIRegister(int_cnt++),
                                arg->getName(), Register::V_IREGISTER, false);
      } else {
        argr = new ParaRegister(offset, 8, arg->getName(),
                                Register::V_IREGISTER, false);
        offset += 8;
      }
      break;
    }
    default:
      assert(0);
    }
    parameters->push_back(unique_ptr<ParaRegister>(argr));
  }
}

MBasicBlock *MFunction::addBasicBlock(string name) {
  auto bb = new MBasicBlock(name);
  basicBlocks->push_back(unique_ptr<MBasicBlock>(bb));
  bb->setFunction(this);
  return bb;
}

void MFunction::setEntry(MBasicBlock *entry) { this->entry = entry; }

MBasicBlock *MFunction::getEntry() { return entry; }

void MFunction::setExit(MBasicBlock *exit) { this->exit = exit; }

MBasicBlock *MFunction::getExit() { return exit; }

void MFunction::setMod(MModule *mod) { this->mod = mod; }

MModule *MFunction::getMod() { return mod; }

ParaRegister *MFunction::getPara(int idx) { return &*parameters->at(idx); }

FuncType *MFunction::getType() { return type; }

string MFunction::getName() const { return name; }

vector<unique_ptr<MBasicBlock>> &MFunction::getBasicBlocks() {
  return *basicBlocks;
}

std::ostream &operator<<(std::ostream &os, const MFunction &obj) {
  os << obj.getName() << ":" << endl;
  for (const auto &bb : *obj.basicBlocks) {
    os << *bb << endl;
  }
  return os;
}

/////////////////////////////////////////////////
//
//                MachineModule
//
/////////////////////////////////////////////////

MModule::MModule() {
  globalVariables = make_unique<vector<unique_ptr<MGlobal>>>();
  functions = make_unique<vector<unique_ptr<MFunction>>>();
}

void MModule::ssa_out() { this->if_ssa = false; }

MFunction *MModule::addFunction(FuncType *funcType, string name) {
  MFunction *func = new MFunction(funcType, name);
  functions->push_back(unique_ptr<MFunction>(func));
  func->setMod(this);
  return func;
}

MGlobal *MModule::addGlobalVariable(GlobalVariable *global) {
  auto g = new MGlobal(global);
  globalVariables->push_back(unique_ptr<MGlobal>(g));
  return g;
}

MGlobal *MModule::addGlobalFloat(FloatConstant *f) {
  static int float_cnt = 0;
  auto g = new MGlobal(
      new GlobalVariable(FloatType::getFloatType(), f, "fi" + float_cnt));
  globalVariables->push_back(unique_ptr<MGlobal>(g));
  return g;
}

vector<unique_ptr<MGlobal>> &MModule::getGlobals() { return *globalVariables; }

vector<unique_ptr<MFunction>> &MModule::getFunctions() { return *functions; }

std::ostream &operator<<(std::ostream &os, const MModule &obj) {
  os << ".globl main\n";
  for (const auto &gv : *obj.globalVariables) {
    os << *gv << endl;
  }
  for (const auto &f : *obj.functions) {
    os << *f << endl;
  }
  return os;
}