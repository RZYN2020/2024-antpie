#ifndef _MACHINE_INSTRUCTION_H_
#define _MACHINE_INSTRUCTION_H_

#include "GlobalVariable.hh"
#include "Register.hh"
#include <cassert>
#include <memory>
#include <vector>

using std::ostream;
using std::unique_ptr;
using std::vector;

class MachineInstruction;

class MachineBasicBlock;

class MachineGlobal;

class MachineFunction;

class MachineInstruction : public VRegister {
public:
  enum MachineInstructionTag {
    // Beacuse we only have 32-bits interger and float in SysY, so we don't need
    // 64-bits-wise instruction provided by RV64GC
    PHI,
    ALLOCA,

    // RV64I
    //// Integer Computation
    ADD,
    ADDI, // to add ra/sp
    ADDIW,
    ADDW,

    SUBW,
    AND,
    OR,
    XOR,
    ANDI,
    ORI,
    XORI,
    // SLLW,
    // SRAW,
    // SRLW,
    // SLLIW,
    // SRAIW,
    // SRLIW,
    // LUI,
    // AUIPC,
    SLT,
    SLTI,
    SLTU,
    SLTIU,
    //// Loads and Stores
    LW,
    SW,
    LD, // to store/load ra/sp
    SD,
    //// Control transfer
    BEQ,
    // BNE,
    // BGE,
    // BLT,
    // BGEU,
    // BLTU,
    // JAL,
    // JALR,

    // RV64M
    MUL,
    MULW,
    // MULHU,
    // MULHSU,
    DIVW,
    REMW,
    // DIVUW,
    // REMUW,

    // RV64F and RV64D
    //// Floating-Point Computation
    FADD_S,
    FSUB_S,
    FMUL_S,
    FDIV_S,
    // FSQRT_S,
    // FMIN_S,
    // FMAX_S,
    // FNMADD_S,
    // FNMSUB_S,
    // FMV_S_X,
    // FMV_X_S,
    //// Load and Store
    FLW,
    FSW,
    //// Conversion
    FCVTS_W,
    // FCVTS_WU,
    FCVTW_S,
    // FCVTWU_S,
    //// Comparison
    FEQ_S,
    FLT_S,
    FLE_S,

    // presudo instruction: https://michaeljclark.github.io/asm.html
    CALL,
    J,
    RET,
    LI,
    MV,
    NOT,
    FMV_S,
  };

private:
  MachineInstructionTag tag;
  Register *target; // target in non-SSA version
  unique_ptr<vector<Register *>> oprands;
  int32_t imm;
  MachineBasicBlock *bb;

public:
  MachineInstruction(MachineInstructionTag tag, string name)
      : VRegister(name), tag(tag) {
    oprands = unique_ptr<vector<Register *>>(new vector<Register *>());
    imm = 0;
    switch (tag) {
    case SW:
    case BEQ:
    case FSW:
    case RET:
      break;
    default: {
      target = this;
      addUse(this);
    }
    }
  }
  MachineInstruction(MachineInstructionTag tag) : VRegister(), tag(tag) {
    oprands = unique_ptr<vector<Register *>>(new vector<Register *>());
    imm = 0;
    switch (tag) {
    case SW:
    case FSW:
    case BEQ:
    case RET:
    case J:
      break;
    default: {
      target = this;
      addUse(this);
    }
    }
  }

  virtual string to_string() const = 0;

  void replaceRegisterUsers(Register *newReg) override {
    auto uses = getUses();
    for (Register *use : uses) {
      if (!dynamic_cast<MachineInstruction *>(use))
        continue;
      auto use_instr = static_cast<MachineInstruction *>(use);
      use_instr->replaceRegister(this, newReg);
    }
    clearUses();
  }

  void replaceIRRegister(map<Instruction *, Register *> instr_map);
  void replaceRegister(Register *oldReg, Register *newReg);
  void replaceWith(vector<MachineInstruction *> instrs);
  void insertBefore(vector<MachineInstruction *> instrs);
  void insertAfter(vector<MachineInstruction *> instrs);

  void setBasicBlock(MachineBasicBlock *bb);
  MachineBasicBlock *getBasicBlock() const { return bb; }

  void pushReg(Register *r);
  int getRegNum() const;
  Register *getReg(int idx) const;

  void setImm(int32_t i);
  int32_t getImm() const;

  void setTarget(Register *reg);
  Register *getTarget();
  string getTargetName() const;

  // string getName() const;

  MachineInstructionTag getTag() const;

  bool is_float() const override;
  bool is_64bit() const;
};

class MIphi : public MachineInstruction {
private:
  unique_ptr<vector<MachineBasicBlock *>> incoming;

public:
  MIphi(string name);
  string to_string() const override;
  void pushIncoming(Register *reg, MachineBasicBlock *bb);
  MachineBasicBlock *getIncomingBlock(int idx) const;
};

class MIalloca : public MachineInstruction {
private:
  uint32_t offset; // from stack allocation place
  uint32_t size;

public:
  MIalloca(uint32_t offset_, uint32_t size_, string name);
  string to_string() const override;
  uint32_t getOffset() const;
};

#define DEFINE_MI_BIN_CLASS(NAME)                                              \
  class MI##NAME : public MachineInstruction {                                 \
  public:                                                                      \
    MI##NAME(Register *reg1, Register *reg2);                                  \
    MI##NAME(Register *reg1, Register *reg2, Register *target);                \
    MI##NAME(Register *reg1, Register *reg2, std::string name);                \
    string to_string() const override;                                         \
  };

#define DEFINE_MI_IMM_CLASS(NAME)                                              \
  class MI##NAME : public MachineInstruction {                                 \
  public:                                                                      \
    MI##NAME(Register *reg, int32_t imm);                                      \
    MI##NAME(Register *reg, int32_t imm, Register *target);                    \
    MI##NAME(Register *reg, int32_t imm, std::string name);                    \
    string to_string() const override;                                         \
  };

#define DEFINE_MIN_UNA_CLASS(NAME)                                             \
  class MI##NAME : public MachineInstruction {                                 \
  public:                                                                      \
    MI##NAME(Register *reg);                                                   \
    MI##NAME(Register *reg, Register *target);                                 \
    MI##NAME(Register *reg, std::string name);                                 \
    string to_string() const override;                                         \
  };

DEFINE_MI_IMM_CLASS(addi)
DEFINE_MI_BIN_CLASS(add)
DEFINE_MI_IMM_CLASS(addiw)
DEFINE_MI_BIN_CLASS(addw)
DEFINE_MI_BIN_CLASS(subw)
DEFINE_MI_BIN_CLASS(and)
DEFINE_MI_IMM_CLASS(andi)
DEFINE_MI_BIN_CLASS(or)
DEFINE_MI_IMM_CLASS(ori)
DEFINE_MI_BIN_CLASS(xor)
DEFINE_MI_IMM_CLASS(xori)
DEFINE_MI_BIN_CLASS(slt)
DEFINE_MI_IMM_CLASS(slti)
DEFINE_MI_BIN_CLASS(sltu)
DEFINE_MI_IMM_CLASS(sltiu)

#define DEFINE_MI_LOAD_CLASS(NAME)                                             \
  class MI##NAME : public MachineInstruction {                                 \
  private:                                                                     \
    MachineGlobal *global = nullptr;                                           \
                                                                               \
  public:                                                                      \
    MI##NAME(MachineGlobal *global);                                           \
    MI##NAME(MachineGlobal *global, std::string name);                         \
    MI##NAME(MachineGlobal *global, Register *target);                         \
    MI##NAME(Register *addr, uint32_t offset);                                 \
    MI##NAME(Register *addr, uint32_t offset, std::string name);               \
    MI##NAME(Register *addr, uint32_t offset, Register *target);               \
    MachineGlobal *getGlobal();                                                \
    std::string to_string() const override;                                    \
  };

#define DEFINE_MI_STORE_CLASS(NAME)                                            \
  class MI##NAME : public MachineInstruction {                                 \
  private:                                                                     \
    MachineGlobal *global = nullptr;                                           \
                                                                               \
  public:                                                                      \
    MI##NAME(Register *val, MachineGlobal *global);                            \
    MI##NAME(Register *val, uint32_t offset, Register *addr);                  \
    MachineGlobal *getGlobal();                                                \
    std::string to_string() const override;                                    \
  };

DEFINE_MI_LOAD_CLASS(lw)
DEFINE_MI_STORE_CLASS(sw)
DEFINE_MI_LOAD_CLASS(ld)
DEFINE_MI_STORE_CLASS(sd)
DEFINE_MI_LOAD_CLASS(flw)
DEFINE_MI_STORE_CLASS(fsw)

class MIbeq : public MachineInstruction {
private:
  MachineBasicBlock *targetBB;

public:
  MIbeq(Register *reg1, Register *reg2, MachineBasicBlock *targetBB);
  MachineBasicBlock *getTargetBB();
  string to_string() const override;
};

DEFINE_MI_BIN_CLASS(mul);
DEFINE_MI_BIN_CLASS(mulw);
DEFINE_MI_BIN_CLASS(divw);
DEFINE_MI_BIN_CLASS(remw);

DEFINE_MI_BIN_CLASS(fadd_s);
DEFINE_MI_BIN_CLASS(fsub_s);
DEFINE_MI_BIN_CLASS(fmul_s);
DEFINE_MI_BIN_CLASS(fdiv_s);

DEFINE_MIN_UNA_CLASS(fcvts_w)
DEFINE_MIN_UNA_CLASS(fcvtw_s)

DEFINE_MI_BIN_CLASS(feq_s);
DEFINE_MI_BIN_CLASS(flt_s);
DEFINE_MI_BIN_CLASS(fle_s);

class MIcall : public MachineInstruction { // presudo
private:
  MachineFunction *func;

public:
  MIcall(MachineFunction *func);
  MachineFunction *getFunc();
  string to_string() const override;
};

class MIj : public MachineInstruction { // presudo
private:
  MachineBasicBlock *targetBB;

public:
  MIj(MachineBasicBlock *targetBB);
  MachineBasicBlock *getTargetBB();
  string to_string() const override;
};
class MIret : public MachineInstruction { // presudo
public:
  MIret();
  string to_string() const override;
};

class MIli : public MachineInstruction { // presudo
public:
  MIli(int32_t imm);
  MIli(int32_t imm, string name);
  MIli(int32_t imm, Register *target);
  string to_string() const override;
};

DEFINE_MIN_UNA_CLASS(mv)
DEFINE_MIN_UNA_CLASS(not )
DEFINE_MIN_UNA_CLASS(fmv_s)

#endif