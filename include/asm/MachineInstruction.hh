#ifndef _MACHINE_INSTRUCTION_H_
#define _MACHINE_INSTRUCTION_H_

#include "MachineType.hh"
#include "Register.hh"

using std::ostream;

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///
///             ASM Instructions
///
///////////////////////////////////////////////////
///////////////////////////////////////////////////

class MachineInstruction : public Register {
public:
  enum MachineInstructionTag {
    // Beacuse we only have 32-bits interger and float in SysY, so we don't need
    // word-wise instruction provided by RV64GC

    PHI,

    // RV64I
    //// Integer Computation
    ADDW,
    SUBW,
    AND,
    OR,
    XOR,
    ANDI,
    ORI,
    XORI,
    SLLW,
    SRAW,
    SRLW,
    SLLIW,
    SRAIW,
    SRLIW,
    LUI,
    AUIPC,
    SLTIU,
    //// Loads and Stores
    LW,
    SW,
    //// Control transfer
    BEQ,
    BNE,
    BGE,
    BLT,
    BGEU,
    BLTU,
    JALR,

    // RV64M
    MULW,
    MULHU,
    MULHSU,
    DIVW,
    REMW,
    DIVUW,
    REMUW,

    // RV64F and RV64D
    //// Floating-Point Computation
    FADD_S,
    FSUB_S,
    FMUL_S,
    FDIV_S,
    FSQRT_S,
    FMIN_S,
    FMAX_S,
    FNMADD_S,
    FNMSUB_S,
    FMV_S_X,
    FMV_X_S,
    //// Load and Store
    FLW,
    FSW,
    //// Conversion
    FCVTS_W,
    FCVTS_WU,
    FCVTW_S,
    FCVTWU_S,
    //// Comparison
    FEQ_S,
    FLT_S,
    FLE_S,
  };

private:
  MachineInstructionTag tag;
  unique_ptr<vector<Register *>> oprands;
  unique_ptr<Immediate> imm;

public:
  void pushReg(Register *r);
  void setImm(Immediate i);
  MachineInstructionTag getTag() const;
  Register *getReg(int idx) const;
  Immediate *getImm() const;
  void printASM(ostream &stream) const;
};

#endif