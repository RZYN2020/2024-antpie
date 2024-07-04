#include "Machine.hh"

//////////////////////////////////////
//////////////////////////////////////
///      MachineInstruction        ///
//////////////////////////////////////
//////////////////////////////////////

void MachineInstruction::replaceIRRegister(
    map<Instruction *, Register *> instr_map) {
  for (auto &opd : *oprands) {
    if (IRRegister *irr = dynamic_cast<IRRegister *>(opd)) {
      assert(irr->getTag() == IR_REGISTER);
      Instruction *inst = irr->ir_reg;
      auto it = instr_map.find(inst);
      if (it != instr_map.end()) {
        opd = it->second;
      } else {
        assert(0);
      }
    }
  }
}

void MachineInstruction::replaceRegister(Register *oldReg, Register *newReg) {
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

void MachineInstruction::replaceWith(vector<MachineInstruction *> instrs) {
  bb->replaceInstructionWith(this, instrs);
}

void MachineInstruction::insertBefore(vector<MachineInstruction *> instrs) {
  bb->insertBeforeInstructionWith(this, instrs);
}

void MachineInstruction::insertAfter(vector<MachineInstruction *> instrs) {
  bb->insertAfterInstructionWith(this, instrs);
}

void MachineInstruction::setBasicBlock(MachineBasicBlock *bb) { this->bb = bb; }

void MachineInstruction::pushReg(Register *r) {
  oprands->push_back(r);
  r->addUse(this);
}

int MachineInstruction::getRegNum() const {
  if (oprands == nullptr) {
    return 0;
  }
  return oprands->size();
}

Register *MachineInstruction::getReg(int idx) const {
  if (oprands == nullptr || idx < 0 || idx >= oprands->size()) {
    return nullptr;
  }
  return (*oprands)[idx];
}

void MachineInstruction::setImm(int32_t i) { imm = i; }

int32_t MachineInstruction::getImm() const { return imm; }

void MachineInstruction::setTarget(Register *reg) {
  if (target != nullptr) {
    target->removeUse(this);
    target = nullptr;
  }
  target = reg;
  reg->addUse(this);
}

Register *MachineInstruction::getTarget() { return target; }

string MachineInstruction::getTargetName() const { return target->getName(); }

// string MachineInstruction::getName() const { return to_string(); }

MachineInstruction::MachineInstructionTag MachineInstruction::getTag() const {
  return tag;
}
bool MachineInstruction::is_float() const {
  switch (tag) {
  case FADD_S:
  case FSUB_S:
  case FMUL_S:
  case FDIV_S:
  // case FSQRT_S:
  // case FMIN_S:
  // case FMAX_S:
  // case FNMADD_S:
  // case FNMSUB_S:
  // case FMV_S_X:
  // case FMV_X_S:
  case FLW:
  case FSW:
  case FCVTS_W:
    // case FCVTS_WU:
    // case FCVTW_S:
    // case FCVTWU_S:
    // case FEQ_S:
    // case FLT_S:
    // case FLE_S:
    return true;
  default:
    return false;
  }
}

bool MachineInstruction::is_64bit() const {
  switch (tag) {
  case ADD:
  case ADDI:
  case LD:
  case MUL:
    return true;
  default:
    return false;
  }
}

//////////////////////////////////////
//////////////////////////////////////
///  Concrete MachineInstructions  ///
//////////////////////////////////////
//////////////////////////////////////

MIalloca::MIalloca(uint32_t offset_, uint32_t size_, string name)
    : MachineInstruction(MachineInstruction::MachineInstructionTag::ALLOCA,
                         name) {
  offset = offset_;
  size = size_;
}

string MIalloca::to_string() const {
  return name + " = alloca " + std::to_string(size);
}

uint32_t MIalloca::getOffset() const { return offset; }

////////////////////////////////////////////
MIphi::MIphi(string name)
    : MachineInstruction(MachineInstruction::MachineInstructionTag::PHI, name) {
  incoming = make_unique<vector<MachineBasicBlock *>>();
}

string MIphi::to_string() const {
  string s = this->getName() + " = phi ";
  for (int i = 0; i < this->getRegNum(); i++) {
    s += "[" + this->getReg(i)->getName() + "," +
         this->getIncomingBlock(i)->getName() + "]";
    if (i < this->getRegNum() - 1) {
      s += ", ";
    }
  }
  return s;
}

void MIphi::pushIncoming(Register *reg, MachineBasicBlock *bb) {
  this->pushReg(reg);
  incoming->push_back(bb);
}

MachineBasicBlock *MIphi::getIncomingBlock(int idx) const {
  if (idx < 0 || idx >= incoming->size()) {
    return nullptr;
  }
  return (*incoming)[idx];
}

////////////////////////////////////////////

#define IMPLEMENT_MI_IMM_CLASS(NAME, TAG, ASM_NAME)                            \
  MI##NAME::MI##NAME(Register *reg, int32_t imm)                               \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(reg);                                                        \
    this->setImm(imm);                                                         \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg, int32_t imm, std::string name)             \
      : MachineInstruction(MachineInstruction::TAG, name) {                    \
    this->pushReg(reg);                                                        \
    this->setImm(imm);                                                         \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg, int32_t imm, Register *target)             \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(reg);                                                        \
    this->setImm(imm);                                                         \
    this->setTarget(target);                                                   \
  }                                                                            \
  std::string MI##NAME::to_string() const {                                    \
    std::string target = this->getTargetName();                                \
    auto reg = this->getReg(0)->getName();                                     \
    auto imm = std::to_string(this->getImm());                                 \
    return #ASM_NAME " " + target + ", " + reg + ", " + imm;                   \
  }

#define IMPLEMENT_MI_BIN_CLASS(NAME, TAG, ASM_NAME)                            \
  MI##NAME::MI##NAME(Register *reg1, Register *reg2)                           \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(reg1);                                                       \
    this->pushReg(reg2);                                                       \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg1, Register *reg2, Register *target)         \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(reg1);                                                       \
    this->pushReg(reg2);                                                       \
    this->setTarget(target);                                                   \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg1, Register *reg2, std::string name)         \
      : MachineInstruction(MachineInstruction::TAG, name) {                    \
    this->pushReg(reg1);                                                       \
    this->pushReg(reg2);                                                       \
  }                                                                            \
  std::string MI##NAME::to_string() const {                                    \
    std::string target = this->getTargetName();                                \
    auto reg1 = this->getReg(0)->getName();                                    \
    auto reg2 = this->getReg(1)->getName();                                    \
    return #ASM_NAME " " + target + ", " + reg1 + ", " + reg2;                 \
  }

#define IMPLEMENT_MI_UNA_CLASS(NAME, TAG, ASM_NAME)                            \
  MI##NAME::MI##NAME(Register *reg)                                            \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(reg);                                                        \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg, Register *target)                          \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(reg);                                                        \
    this->setTarget(target);                                                   \
  }                                                                            \
  MI##NAME::MI##NAME(Register *reg, std::string name)                          \
      : MachineInstruction(MachineInstruction::TAG, name) {                    \
    this->pushReg(reg);                                                        \
  }                                                                            \
  std::string MI##NAME::to_string() const {                                    \
    std::string target = this->getTargetName();                                \
    auto reg = this->getReg(0)->getName();                                     \
    return #ASM_NAME " " + target + ", " + reg;                                \
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
  MI##NAME::MI##NAME(MachineGlobal *global)                                    \
      : MachineInstruction(MachineInstruction::TAG), global(global) {}         \
                                                                               \
  MI##NAME::MI##NAME(MachineGlobal *global, std::string name)                  \
      : MachineInstruction(MachineInstruction::TAG, name), global(global) {}   \
                                                                               \
  MI##NAME::MI##NAME(MachineGlobal *global, Register *target)                  \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->setTarget(target);                                                   \
    this->global = global;                                                     \
  }                                                                            \
                                                                               \
  MI##NAME::MI##NAME(Register *addr, uint32_t offset)                          \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(addr);                                                       \
    this->setImm(offset);                                                      \
  }                                                                            \
                                                                               \
  MI##NAME::MI##NAME(Register *addr, uint32_t offset, std::string name)        \
      : MachineInstruction(MachineInstruction::TAG, name) {                    \
    this->pushReg(addr);                                                       \
    this->setImm(offset);                                                      \
  }                                                                            \
                                                                               \
  MI##NAME::MI##NAME(Register *addr, uint32_t offset, Register *target)        \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(addr);                                                       \
    this->setTarget(target);                                                   \
    this->setImm(offset);                                                      \
  }                                                                            \
                                                                               \
  MachineGlobal *MI##NAME::getGlobal() { return this->global; }                \
                                                                               \
  std::string MI##NAME::to_string() const {                                    \
    if (this->global) {                                                        \
      return #NAME " " + this->getTargetName() + ", " +                        \
             this->global->getName();                                          \
    } else {                                                                   \
      return #NAME " " + this->getTargetName() + ", " +                        \
             std::to_string(getImm()) + "(" + this->getReg(0)->getName() +     \
             ")";                                                              \
    }                                                                          \
  }

#define IMPLEMENT_MI_STORE_CLASS(NAME, TAG)                                    \
  MI##NAME::MI##NAME(Register *val, MachineGlobal *global)                     \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(val);                                                        \
    this->global = global;                                                     \
  }                                                                            \
                                                                               \
  MI##NAME::MI##NAME(Register *val, uint32_t offset, Register *addr)           \
      : MachineInstruction(MachineInstruction::TAG) {                          \
    this->pushReg(val);                                                        \
    this->pushReg(addr);                                                       \
    this->setImm(offset);                                                      \
  }                                                                            \
                                                                               \
  MachineGlobal *MI##NAME::getGlobal() { return this->global; }                \
                                                                               \
  std::string MI##NAME::to_string() const {                                    \
    if (this->global) {                                                        \
      return #NAME " " + this->getReg(0)->getName() + ", " +                   \
             this->global->getName();                                          \
    } else {                                                                   \
      return #NAME " " + this->getReg(0)->getName() + ", " +                   \
             std::to_string(getImm()) + "(" +                         \
             this->getReg(1)->getName() + ")";                                 \
    }                                                                          \
  }

IMPLEMENT_MI_LOAD_CLASS(lw, LW)
IMPLEMENT_MI_STORE_CLASS(sw, SW)

IMPLEMENT_MI_LOAD_CLASS(ld, LD)
IMPLEMENT_MI_STORE_CLASS(sd, SD)

IMPLEMENT_MI_LOAD_CLASS(flw, FLW)
IMPLEMENT_MI_STORE_CLASS(fsw, FSW)

// MIbeq
MIbeq::MIbeq(Register *reg1, Register *reg2, MachineBasicBlock *targetBB)
    : MachineInstruction(MachineInstruction::BEQ), targetBB(targetBB) {
  this->pushReg(reg1);
  this->pushReg(reg2);
}

MachineBasicBlock *MIbeq::getTargetBB() { return this->targetBB; }

std::string MIbeq::to_string() const {
  return "beq " + this->getReg(0)->getName() + ", " +
         this->getReg(1)->getName() + ", " + this->targetBB->getName();
}

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

// MIcall
MIcall::MIcall(MachineFunction *func)
    : MachineInstruction(MachineInstruction::CALL), func(func) {}

MachineFunction *MIcall::getFunc() { return this->func; }

std::string MIcall::to_string() const {
  return "call " + this->func->getName();
}

// MIj
MIj::MIj(MachineBasicBlock *targetBB)
    : MachineInstruction(MachineInstruction::J), targetBB(targetBB) {}

MachineBasicBlock *MIj::getTargetBB() { return this->targetBB; }

std::string MIj::to_string() const { return "j " + this->targetBB->getName(); }

// MIret
MIret::MIret() : MachineInstruction(MachineInstruction::RET) {}

std::string MIret::to_string() const { return "ret"; }

// MIli
MIli::MIli(int32_t imm) : MachineInstruction(MachineInstruction::LI) {
  this->setImm(imm);
}

MIli::MIli(int32_t imm, string name)
    : MachineInstruction(MachineInstruction::LI, name) {
  this->setImm(imm);
}

MIli::MIli(int32_t imm, Register *target)
    : MachineInstruction(MachineInstruction::LI, name) {
  this->setImm(imm);
  this->setTarget(target);
}

std::string MIli::to_string() const {
  return "li " + this->getTargetName() + ", " + std::to_string(getImm());
}

IMPLEMENT_MI_UNA_CLASS(mv, MV, mv)
IMPLEMENT_MI_UNA_CLASS(not, NOT, not )
IMPLEMENT_MI_UNA_CLASS(fmv_s, FMV_S, fmv.s)
