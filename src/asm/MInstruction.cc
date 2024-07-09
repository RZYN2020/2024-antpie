#include "Machine.hh"

//////////////////////////////////////
//////////////////////////////////////
///      MachineInstruction        ///
//////////////////////////////////////
//////////////////////////////////////

MInstruction::MInstruction(MITag tag, string name) : VRegister(name) {
  oprands = make_unique<vector<Register *>>();
  this->tag = tag;
}

MInstruction::MInstruction(MITag tag) : VRegister() {
  oprands = make_unique<vector<Register *>>();
  this->tag = tag;
}

void MInstruction::setTarget(Register *reg) {
  if (target != nullptr) {
    target->removeUse(this);
    target = nullptr;
  }
  target = reg;
  target->addUse(this);
}

Register *MInstruction::getTarget() { return target; }

void MInstruction::setComment(string comment) { this->comment = comment; }

string MInstruction::getComment() { return comment; }

void MInstruction::replaceIRRegister(map<Instruction *, Register *> instr_map) {
  for (auto &opd : *oprands) {
    if (opd->getTag() == IR_REGISTER) {
      auto irr = static_cast<IRRegister *>(opd);
      Instruction *inst = irr->ir_reg;
      auto it = instr_map.find(inst);
      if (it != instr_map.end()) {
        opd = it->second;
        delete irr;
      } else {
        assert(0);
      }
    }
  }
}

void MInstruction::replaceRegister(Register *oldReg, Register *newReg) {
  if (target == oldReg) {
    target = newReg;
  }

  for (auto it = oprands->begin(); it != oprands->end(); ++it) {
    if (*it == oldReg) {
      *it = newReg;
    }
  }
  oldReg->removeUse(this);
  newReg->addUse(this);
}

void MInstruction::replaceWith(vector<MInstruction *> instrs) {
  bb->replaceInstructionWith(this, instrs);
}

void MInstruction::insertBefore(vector<MInstruction *> instrs) {
  bb->insertBeforeInstructionWith(this, instrs);
}

void MInstruction::insertAfter(vector<MInstruction *> instrs) {
  bb->insertAfterInstructionWith(this, instrs);
}

void MInstruction::setBasicBlock(MBasicBlock *bb) { this->bb = bb; }

void MInstruction::pushReg(Register *r) {
  oprands->push_back(r);
  r->addUse(this);
}

int MInstruction::getRegNum() const {
  if (oprands == nullptr) {
    return 0;
  }
  return oprands->size();
}

Register *MInstruction::getReg(int idx) const {
  if (oprands == nullptr || idx < 0 || idx >= oprands->size()) {
    return nullptr;
  }
  return (*oprands)[idx];
}

MInstruction::MITag MInstruction::getInsTag() const { return tag; }

std::ostream &operator<<(std::ostream &os, MInstruction &obj) {
  return obj.printASM(os);
}

//////////////////////////////////////
//////////////////////////////////////
///  Concrete MachineInstructions  ///
//////////////////////////////////////
//////////////////////////////////////

MHIphi::MHIphi(string name) : MInstruction(MInstruction::MITag::H_PHI, name) {
  incoming = make_unique<vector<MBasicBlock *>>();
  setTarget(this);
}

ostream &MHIphi::printASM(ostream &os) {
  os << getTarget()->getName() << " = phi ";
  for (int i = 0; i < this->getRegNum(); i++) {
    os << "[" << this->getReg(i)->getName() << ","
       << this->getIncomingBlock(i)->getName() + "]";
    if (i < this->getRegNum() - 1) {
      os << ", ";
    }
  }
  return os;
}

void MHIphi::pushIncoming(Register *reg, MBasicBlock *bb) {
  this->pushReg(reg);
  incoming->push_back(bb);
}

void MHIphi::replaceIncoming(MBasicBlock *oldbb, MBasicBlock *newbb) {
  auto it = incoming->begin();
  while (it != incoming->end()) {
    if (*it == oldbb) {
      *it = newbb;
      break;
    }
    ++it;
  }
}

MBasicBlock *MHIphi::getIncomingBlock(int idx) const {
  if (idx < 0 || idx >= incoming->size()) {
    return nullptr;
  }
  return (*incoming)[idx];
}

////////////////////////////////////////////

MHIalloca::MHIalloca(uint32_t size_, string name)
    : MInstruction(MInstruction::MITag::H_ALLOCA, name) {
  size = size_;
  this->setTarget(this);
}

ostream &MHIalloca::printASM(ostream &os) {
  return os << getTarget()->getName() << " = alloca " << std::to_string(size);
}
////////////////////////////////////////////

MHIret::MHIret(int imm) : MInstruction(MInstruction::MITag::H_RET) {
  this->imm = imm;
  this->ret_type = Int;
}

MHIret::MHIret(float imm) : MInstruction(MInstruction::MITag::H_RET) {
  this->fimm = imm;
  this->ret_type = Int;
}
MHIret::MHIret(Register *reg) : MInstruction(MInstruction::MITag::H_RET) {
  pushReg(reg);
  this->ret_type = Reg;
}
ostream &MHIret::printASM(ostream &os) {
  os << "ret ";
  switch (ret_type) {
  case Float:
    os << fimm;
    break;
  case Int:
    os << imm;
    break;
  case Reg:
    os << getReg(0)->getName();
    break;
  }
  return os;
}

////////////////////////////////////////////

MHIcall::MHIcall(MFunction *func, string name)
    : MInstruction(MInstruction::MITag::H_CALL, name) {
  this->function = func;
  this->setTarget(this);
  this->args = make_unique<vector<unique_ptr<CallArg>>>();
}

MHIcall::MHIcall(MFunction *func) : MInstruction(MInstruction::MITag::H_CALL) {
  this->function = func;
  this->args = make_unique<vector<unique_ptr<CallArg>>>();
}

void MHIcall::pushArg(float f) {
  auto arg = make_unique<CallArg>();
  arg->tp = ArgTp::Float;
  arg->arg.f = f;
  this->args->push_back(move(arg));
}

void MHIcall::pushArg(int i) {
  auto arg = make_unique<CallArg>();
  arg->tp = ArgTp::Int;
  arg->arg.i = i;
  this->args->push_back(move(arg));
}

void MHIcall::pushArg(Register *r) {
  auto arg = make_unique<CallArg>();
  arg->tp = ArgTp::Reg;
  arg->arg.reg = r;
  this->args->push_back(move(arg));
}

int MHIcall::getArgNum() { return this->args->size(); }

MHIcall::CallArg &MHIcall::getArg(int idx) {
  if (idx >= this->args->size()) {
    assert(0);
  }
  return *args->at(idx);
}

ostream &MHIcall::printASM(ostream &os) {
  auto target = getTarget();
  if (target != nullptr) {
    os << target->getName() << " = ";
  }
  os << "call " << this->function->getName() << "(";
  for (int i = 0; i < this->args->size(); i++) {
    auto &arg = this->args->at(i);
    switch (arg->tp) {
    case ArgTp::Float:
      os << arg->arg.f;
      break;
    case ArgTp::Int:
      os << arg->arg.i;
      break;
    case ArgTp::Reg:
      os << arg->arg.reg->getName();
      break;
    }
    if (i < this->args->size() - 1) {
      os << ", ";
    }
  }
  os << ")";
  return os;
}

MHIicmp::MHIicmp(OpTag optag, Register *reg1, Register *reg2, std::string name)
    : MInstruction(MInstruction::MITag::H_ICMP, name) {
  this->optag = optag;
  pushReg(reg1);
  pushReg(reg2);
  this->setTarget(this);
}

OpTag MHIicmp::getOpTag() { return optag; }

ostream &MHIicmp::printASM(ostream &os) {
  auto target = this->getTarget()->getName();
  auto reg1 = this->getReg(0)->getName();
  auto reg2 = this->getReg(1)->getName();
  switch (optag) {
  case OpTag::EQ: {
    os << "h_eq ";
    break;
  }
  case OpTag::NE: {
    os << "h_ne ";
    break;
  }
  case OpTag::SLE: {
    os << "h_le ";
    break;
  }
  case OpTag::SLT: {
    os << "h_lt ";
    break;
  }
  case OpTag::SGE: {
    os << "h_ge ";
    break;
  }
  case OpTag::SGT: {
    os << "h_gt ";
    break;
  }
  default:
    assert(0);
  }
  return os << reg1 << ", " << reg2;
}

////////////////////////////////////////////

MHIbr::MHIbr(Register *reg, MBasicBlock *t_bb, MBasicBlock *f_bb)
    : MInstruction(MInstruction::MITag::H_BR) {
  pushReg(reg);
  this->t_bb = t_bb;
  this->f_bb = f_bb;
}

void MHIbr::setTBlock(MBasicBlock *bb) { this->t_bb = bb; }
void MHIbr::setFBlock(MBasicBlock *bb) { this->f_bb = bb; }
MBasicBlock *MHIbr::getTBlock() { return t_bb; }
MBasicBlock *MHIbr::getFBlock() { return f_bb; }
ostream &MHIbr::printASM(ostream &os) {
  return os << "br " << this->getReg(0)->getName() << ", "
            << this->t_bb->getName() << ", " << this->f_bb->getName();
}

////////////////////////////////////////////

#define IMPLEMENT_MI_IMM_CLASS(NAME, TAG, ASM_NAME)                            \
  MI##NAME::MI##NAME(Register *reg, int32_t imm)                               \
      : MInstruction(MInstruction::TAG) {                                      \
    this->pushReg(reg);                                                        \
    this->imm = imm;                                                           \
    this->setTarget(this);                                                     \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg, int32_t imm, std::string name)             \
      : MInstruction(MInstruction::TAG, name) {                                \
    this->pushReg(reg);                                                        \
    this->imm = imm;                                                           \
    this->setTarget(this);                                                     \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg, int32_t imm, Register *target)             \
      : MInstruction(MInstruction::TAG) {                                      \
    this->pushReg(reg);                                                        \
    this->imm = imm;                                                           \
    this->setTarget(target);                                                   \
  }                                                                            \
  ostream &MI##NAME::printASM(ostream &os) {                                   \
    auto target = this->getTarget()->getName();                                \
    auto reg = this->getReg(0)->getName();                                     \
    auto imm = std::to_string(this->imm);                                      \
    return os << #ASM_NAME << " " << target << ", " << reg << ", " << imm;     \
  }

#define IMPLEMENT_MI_BIN_CLASS(NAME, TAG, ASM_NAME)                            \
  MI##NAME::MI##NAME(Register *reg1, Register *reg2)                           \
      : MInstruction(MInstruction::TAG) {                                      \
    this->pushReg(reg1);                                                       \
    this->pushReg(reg2);                                                       \
    this->setTarget(this);                                                     \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg1, Register *reg2, Register *target)         \
      : MInstruction(MInstruction::TAG) {                                      \
    this->pushReg(reg1);                                                       \
    this->pushReg(reg2);                                                       \
    this->setTarget(target);                                                   \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg1, Register *reg2, std::string name)         \
      : MInstruction(MInstruction::TAG, name) {                                \
    this->pushReg(reg1);                                                       \
    this->pushReg(reg2);                                                       \
    this->setTarget(this);                                                     \
  }                                                                            \
  ostream &MI##NAME::printASM(ostream &os) {                                   \
    auto target = this->getTarget()->getName();                                \
    auto reg1 = this->getReg(0)->getName();                                    \
    auto reg2 = this->getReg(1)->getName();                                    \
    return os << #ASM_NAME << " " << target << ", " << reg1 << ", " << reg2;   \
  }

#define IMPLEMENT_MI_UNA_CLASS(NAME, TAG, ASM_NAME)                            \
  MI##NAME::MI##NAME(Register *reg) : MInstruction(MInstruction::TAG) {        \
    this->pushReg(reg);                                                        \
    this->setTarget(this);                                                     \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg, Register *target)                          \
      : MInstruction(MInstruction::TAG) {                                      \
    this->pushReg(reg);                                                        \
    this->setTarget(target);                                                   \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg, std::string name)                          \
      : MInstruction(MInstruction::TAG, name) {                                \
    this->pushReg(reg);                                                        \
    this->setTarget(this);                                                     \
  }                                                                            \
  ostream &MI##NAME::printASM(ostream &os) {                                   \
    auto target = this->getTarget()->getName();                                \
    auto reg = this->getReg(0)->getName();                                     \
    return os << #ASM_NAME << " " << target << ", " << reg;                    \
  }

IMPLEMENT_MI_IMM_CLASS(addi, ADDI, addi)
IMPLEMENT_MI_BIN_CLASS(add, ADD, add)
IMPLEMENT_MI_IMM_CLASS(addiw, ADDIW, addiw)
IMPLEMENT_MI_BIN_CLASS(addw, ADDW, addw)
IMPLEMENT_MI_BIN_CLASS(subw, SUBW, subw)
IMPLEMENT_MI_BIN_CLASS(and, AND, and)
IMPLEMENT_MI_IMM_CLASS(andi, ANDI, andi)
IMPLEMENT_MI_BIN_CLASS(or, OR, or)
IMPLEMENT_MI_IMM_CLASS(ori, ORI, ori)
IMPLEMENT_MI_BIN_CLASS(xor, XOR, xor)
IMPLEMENT_MI_IMM_CLASS(xori, XORI, xori)
IMPLEMENT_MI_BIN_CLASS(slt, SLT, slt)
IMPLEMENT_MI_IMM_CLASS(slti, SLTI, slti)
IMPLEMENT_MI_BIN_CLASS(sltu, SLTU, sltu)
IMPLEMENT_MI_IMM_CLASS(sltiu, SLTIU, sltiu)

#define IMPLEMENT_MI_LOAD_CLASS(NAME, TAG)                                     \
  MI##NAME::MI##NAME(MGlobal *global)                                          \
      : MInstruction(MInstruction::TAG), global(global) {}                     \
                                                                               \
  MI##NAME::MI##NAME(MGlobal *global, std::string name)                        \
      : MInstruction(MInstruction::TAG, name), global(global) {                \
    this->setTarget(this);                                                     \
  }                                                                            \
                                                                               \
  MI##NAME::MI##NAME(MGlobal *global, Register *target)                        \
      : MInstruction(MInstruction::TAG) {                                      \
    this->setTarget(target);                                                   \
    this->global = global;                                                     \
  }                                                                            \
                                                                               \
  MI##NAME::MI##NAME(Register *addr, int offset)                               \
      : MInstruction(MInstruction::TAG) {                                      \
    this->pushReg(addr);                                                       \
    imm = offset;                                                              \
    this->setTarget(this);                                                     \
  }                                                                            \
                                                                               \
  MI##NAME::MI##NAME(Register *addr, int offset, std::string name)             \
      : MInstruction(MInstruction::TAG, name) {                                \
    this->pushReg(addr);                                                       \
    imm = offset;                                                              \
    this->setTarget(this);                                                     \
  }                                                                            \
                                                                               \
  MI##NAME::MI##NAME(Register *addr, int offset, Register *target)             \
      : MInstruction(MInstruction::TAG) {                                      \
    this->pushReg(addr);                                                       \
    this->setTarget(target);                                                   \
    imm = offset;                                                              \
  }                                                                            \
                                                                               \
  MGlobal *MI##NAME::getGlobal() { return this->global; }                      \
                                                                               \
  ostream &MI##NAME::printASM(ostream &os) {                                   \
    if (this->global) {                                                        \
      return os << #NAME << " " << this->getTarget()->getName() << +", "       \
                << this->global->getName();                                    \
    } else {                                                                   \
      return os << #NAME << " " << this->getTarget()->getName() << ", "        \
                << std::to_string(imm) << "(" << this->getReg(0)->getName()    \
                << ")";                                                        \
    }                                                                          \
  }

#define IMPLEMENT_MI_STORE_CLASS(NAME, TAG)                                    \
  MI##NAME::MI##NAME(Register *val, MGlobal *global)                           \
      : MInstruction(MInstruction::TAG) {                                      \
    this->pushReg(val);                                                        \
    this->global = global;                                                     \
  }                                                                            \
                                                                               \
  MI##NAME::MI##NAME(Register *val, int offset, Register *addr)                \
      : MInstruction(MInstruction::TAG) {                                      \
    this->pushReg(val);                                                        \
    this->pushReg(addr);                                                       \
    imm = offset;                                                              \
  }                                                                            \
                                                                               \
  MGlobal *MI##NAME::getGlobal() { return this->global; }                      \
                                                                               \
  ostream &MI##NAME::printASM(ostream &os) {                                   \
    if (this->global) {                                                        \
      return os << #NAME << " " << this->getReg(0)->getName() << ", "          \
                << this->global->getName();                                    \
    } else {                                                                   \
      return os << #NAME << " " << this->getReg(0)->getName() << ", "          \
                << std::to_string(imm) << "(" << this->getReg(1)->getName()    \
                << ")";                                                        \
    }                                                                          \
  }

IMPLEMENT_MI_LOAD_CLASS(lw, LW)
IMPLEMENT_MI_STORE_CLASS(sw, SW)

IMPLEMENT_MI_LOAD_CLASS(ld, LD)
IMPLEMENT_MI_STORE_CLASS(sd, SD)

IMPLEMENT_MI_LOAD_CLASS(flw, FLW)
IMPLEMENT_MI_STORE_CLASS(fsw, FSW)

IMPLEMENT_MI_BIN_CLASS(mul, MUL, mul)
IMPLEMENT_MI_BIN_CLASS(mulw, MULW, mulw)
IMPLEMENT_MI_BIN_CLASS(divw, DIVW, divw)
IMPLEMENT_MI_BIN_CLASS(remw, REMW, remw)

IMPLEMENT_MI_BIN_CLASS(fadd_s, FADD_S, fadd.s)
IMPLEMENT_MI_BIN_CLASS(fsub_s, FSUB_S, fsub.s)
IMPLEMENT_MI_BIN_CLASS(fmul_s, FMUL_S, fmul.s)
IMPLEMENT_MI_BIN_CLASS(fdiv_s, FDIV_S, fdiv.s)

IMPLEMENT_MI_UNA_CLASS(fcvts_w, FCVTS_W, fcvt.s.w)
IMPLEMENT_MI_UNA_CLASS(fcvtw_s, FCVTW_S, fcvt.w.s)

IMPLEMENT_MI_BIN_CLASS(feq_s, FEQ_S, feq.s)
IMPLEMENT_MI_BIN_CLASS(flt_s, FLT_S, flt.s)
IMPLEMENT_MI_BIN_CLASS(fle_s, FLE_S, fle.s)

#define IMPLEMENT_MIT_BRANCH_CLASS(NAME, TAG)                                  \
  MI##NAME::MI##NAME(Register *reg1, Register *reg2, MBasicBlock *targetBB)    \
      : MInstruction(MInstruction::TAG), targetBB(targetBB) {                  \
    this->pushReg(reg1);                                                       \
    this->pushReg(reg2);                                                       \
  }                                                                            \
                                                                               \
  MBasicBlock *MI##NAME::getTargetBB() { return this->targetBB; }              \
  void MI##NAME::setTargetBB(MBasicBlock *bb) { this->targetBB = bb; }         \
  ostream &MI##NAME::printASM(ostream &os) {                                   \
    return os << #NAME << " " << this->getReg(0)->getName() << ", "            \
              << this->getReg(1)->getName() << ", "                            \
              << this->targetBB->getName();                                    \
  }

IMPLEMENT_MIT_BRANCH_CLASS(beq, BEQ)
IMPLEMENT_MIT_BRANCH_CLASS(bne, BNE)
IMPLEMENT_MIT_BRANCH_CLASS(bge, BGE)
IMPLEMENT_MIT_BRANCH_CLASS(blt, BLT)

// MIj
MIj::MIj(MBasicBlock *targetBB)
    : MInstruction(MInstruction::J), targetBB(targetBB) {}

void MIj::setTargetBB(MBasicBlock *bb) { this->targetBB = bb; }
MBasicBlock *MIj::getTargetBB() { return this->targetBB; }

ostream &MIj::printASM(ostream &os) {
  return os << "j " << this->targetBB->getName();
}

// MIcall
MIcall::MIcall(MFunction *func)
    : MInstruction(MInstruction::CALL), func(func) {}

MFunction *MIcall::getFunc() { return this->func; }

ostream &MIcall::printASM(ostream &os) {
  return os << "call " << this->func->getName();
}

// MIret
MIret::MIret() : MInstruction(MInstruction::RET) {}

ostream &MIret::printASM(ostream &os) { return os << "ret"; }

// MIli
MIli::MIli(int32_t imm) : MInstruction(MInstruction::LI) {
  this->imm = imm;
  this->setTarget(this);
}

MIli::MIli(int32_t imm, string name) : MInstruction(MInstruction::LI, name) {
  this->imm = imm;
  this->setTarget(this);
}

MIli::MIli(int32_t imm, Register *target) : MInstruction(MInstruction::LI) {
  this->imm = imm;
  this->setTarget(target);
}

ostream &MIli::printASM(ostream &os) {
  return os << "li "
            << this->getTarget()->getName() + ", " + std::to_string(imm);
}

IMPLEMENT_MI_UNA_CLASS(mv, MV, mv)
IMPLEMENT_MI_UNA_CLASS(fmv_s, FMV_S, fmv.s)

Mcomment::Mcomment(string comment) : MInstruction(MInstruction::COMMENT) {
  this->comment = comment;
}
ostream &Mcomment::printASM(ostream &os) { return os << "# " << comment; }