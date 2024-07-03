#include "MachineInstruction.hh"
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

#define DEFINE_MI_IMM_CLASS_IMPL(NAME, TAG, ASM_NAME)                          \
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

#define DEFINE_MI_BIN_CLASS_IMPL(NAME, TAG, ASM_NAME)                          \
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

#define DEFINE_MI_UNA_CLASS_IMPL(NAME, TAG, ASM_NAME)                          \
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
    return #ASM_NAME " " + target + ", " + reg;                                 \
  }

DEFINE_MI_IMM_CLASS_IMPL(addiw, ADDIW, addiw)
DEFINE_MI_BIN_CLASS_IMPL(addw, ADDW, addw)
DEFINE_MI_BIN_CLASS_IMPL(subw, SUBW, subw)
DEFINE_MI_BIN_CLASS_IMPL(and, AND, and)
DEFINE_MI_IMM_CLASS_IMPL(andi, ANDI, andi)
DEFINE_MI_BIN_CLASS_IMPL(or, OR, or)
DEFINE_MI_IMM_CLASS_IMPL(ori, ORI, ori)
DEFINE_MI_BIN_CLASS_IMPL(xor, XOR, xor)
DEFINE_MI_IMM_CLASS_IMPL(xori, XORI, xori)
DEFINE_MI_BIN_CLASS_IMPL(slt, SLT, slt)
DEFINE_MI_IMM_CLASS_IMPL(slti, SLTI, slti)
DEFINE_MI_BIN_CLASS_IMPL(sltu, SLTU, sltu)
DEFINE_MI_IMM_CLASS_IMPL(sltiu, SLTIU, sltiu)

// MIlw
MIlw::MIlw(MachineGlobal *global)
    : MachineInstruction(MachineInstruction::LW), global(global) {}

MIlw::MIlw(MachineGlobal *global, string name)
    : MachineInstruction(MachineInstruction::LW, name), global(global) {}

MIlw::MIlw(MachineGlobal *global, Register *target)
    : MachineInstruction(MachineInstruction::LW), global(global) {
  this->setTarget(target);
}

MIlw::MIlw(Register *addr) : MachineInstruction(MachineInstruction::LW) {
  this->pushReg(addr);
}

MIlw::MIlw(Register *addr, string name)
    : MachineInstruction(MachineInstruction::LW, name) {
  this->pushReg(addr);
}

MIlw::MIlw(Register *addr, Register *target)
    : MachineInstruction(MachineInstruction::LW) {
  this->pushReg(addr);
  this->setTarget(target);
}

MachineGlobal *MIlw::getGlobal() { return this->global; }

std::string MIlw::to_string() const {
  if (this->global) {
    return "lw " + this->getTargetName() + ", " + this->global->getName();
  } else {
    auto offset = getBasicBlock()->getFunction()->getSavedSize();
    return "lw " + this->getTargetName() + ", " +
           std::to_string(getImm() + offset) + "(" +
           this->getReg(0)->getName() + ")";
  }
}

// MIsw
MIsw::MIsw(MachineGlobal *global, Register *val)
    : MachineInstruction(MachineInstruction::SW), global(global) {
  this->pushReg(val);
}

MIsw::MIsw(Register *addr, Register *val)
    : MachineInstruction(MachineInstruction::SW) {
  this->pushReg(addr);
  this->pushReg(val);
}

std::string MIsw::to_string() const {
  if (this->global) {
    return "sw " + this->getReg(0)->getName() + ", " + this->global->getName();
  } else {
    auto offset = getBasicBlock()->getFunction()->getSavedSize();
    return "sw " + this->getReg(1)->getName() + ", " +
           std::to_string(getImm() + offset) + "(" +
           this->getReg(0)->getName() + ")";
  }
}

// MIsd
MIsd::MIsd(Register *addr, uint32_t offset, Register *val)
    : MachineInstruction(MachineInstruction::SD) {
  this->pushReg(addr);
  this->pushReg(val);
  this->setImm(offset);
}

std::string MIsd::to_string() const {
  return "sd " + this->getReg(1)->getName() + ", " + std::to_string(getImm()) +
         "(" + this->getReg(0)->getName() + ")";
}

// MIld
MIld::MIld(Register *addr, uint32_t offset, Register *target)
    : MachineInstruction(MachineInstruction::LD) {
  this->pushReg(addr);
  this->pushReg(target);
  this->setImm(offset);
}

std::string MIld::to_string() const {
  return "ld " + this->getReg(1)->getName() + ", " + std::to_string(getImm()) +
         "(" + this->getReg(0)->getName() + ")";
}

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

DEFINE_MI_BIN_CLASS_IMPL(mulw, MULW, mulw)
DEFINE_MI_BIN_CLASS_IMPL(divw, DIVW, divw)
DEFINE_MI_BIN_CLASS_IMPL(remw, REMW, remw)

DEFINE_MI_BIN_CLASS_IMPL(fadd_s, FADD_S, fadd.s)
DEFINE_MI_BIN_CLASS_IMPL(fsub_s, FSUB_S, fsub.s)
DEFINE_MI_BIN_CLASS_IMPL(fmul_s, FMUL_S, fmul.s)
DEFINE_MI_BIN_CLASS_IMPL(fdiv_s, FDIV_S, fdiv.s)

MIflw::MIflw(MachineGlobal *global)
    : MachineInstruction(MachineInstruction::FLW), global(global) {}

MIflw::MIflw(MachineGlobal *global, string name)
    : MachineInstruction(MachineInstruction::FLW, name), global(global) {}

MIflw::MIflw(MachineGlobal *global, Register *target)
    : MachineInstruction(MachineInstruction::FLW), global(global) {
  this->setTarget(target);
}

MIflw::MIflw(Register *addr) : MachineInstruction(MachineInstruction::FLW) {
  this->pushReg(addr);
}

MIflw::MIflw(Register *addr, string name)
    : MachineInstruction(MachineInstruction::FLW, name) {
  this->pushReg(addr);
}

MIflw::MIflw(Register *addr, Register *target)
    : MachineInstruction(MachineInstruction::FLW) {
  this->pushReg(addr);
  this->setTarget(target);
}

MachineGlobal *MIflw::getGlobal() { return this->global; }

std::string MIflw::to_string() const {
  if (this->global) {
    return "flw " + this->getTargetName() + ", " + this->global->getName();
  } else {
    auto offset = getBasicBlock()->getFunction()->getSavedSize();
    return "flw " + this->getTargetName() + ", " +
           std::to_string(getImm() + offset) + "(" +
           this->getReg(0)->getName() + ")";
  }
}

// MIfsw
MIfsw::MIfsw(MachineGlobal *global, Register *val)
    : MachineInstruction(MachineInstruction::FSW), global(global) {
  this->pushReg(val);
}

MIfsw::MIfsw(Register *addr, Register *val)
    : MachineInstruction(MachineInstruction::FSW) {
  this->pushReg(addr);
  this->pushReg(val);
}

std::string MIfsw::to_string() const {
  if (this->global) {
    return "fsw " + this->getReg(0)->getName() + ", " + this->global->getName();
  } else {
    auto offset = getBasicBlock()->getFunction()->getSavedSize();
    return "fsw " + this->getReg(1)->getName() + ", " +
           std::to_string(getImm() + offset) + "(" +
           this->getReg(0)->getName() + ")";
  }
}

DEFINE_MI_UNA_CLASS_IMPL(fcvts_w, FCVTS_W, fcvt.s.w)
DEFINE_MI_UNA_CLASS_IMPL(fcvtw_s, FCVTW_S, fcvt.w.s)

DEFINE_MI_BIN_CLASS_IMPL(feq_s, FEQ_S, feq.s)
DEFINE_MI_BIN_CLASS_IMPL(flt_s, FLT_S, flt.s)
DEFINE_MI_BIN_CLASS_IMPL(fle_s, FLE_S, fle.s)

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

DEFINE_MI_UNA_CLASS_IMPL(mv, MV, mv)
DEFINE_MI_UNA_CLASS_IMPL(not, NOT, not )
DEFINE_MI_UNA_CLASS_IMPL(fmv_s, FMV_S, fmv.s)
