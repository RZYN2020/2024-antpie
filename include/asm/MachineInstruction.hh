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

class MachineBasicBlock {
private:
  string name;
  unique_ptr<vector<unique_ptr<MachineInstruction>>> instructions;

public:
  MachineBasicBlock(string name_) {
    name = name_;
    instructions = make_unique<vector<unique_ptr<MachineInstruction>>>();
  }
  void pushInstr(MachineInstruction *i) {
    instructions->push_back(unique_ptr<MachineInstruction>(i));
  }

  void pushInstrs(vector<MachineInstruction *> is) {
    for (auto i : is) {
      instructions->push_back(unique_ptr<MachineInstruction>(i));
    }
  }

  void printASM(ostream &stream) const;

  const vector<unique_ptr<MachineInstruction>> &getInstructions() const {
    return *instructions;
  }
};

class MachineGlobal {
private:
  GlobalVariable *global;

public:
  MachineGlobal(GlobalVariable *global) : global(global) {}
  void printASM(ostream &stream) const;
};

class MachineFunction;

class MachineInstruction : public VRegister {
public:
  enum MachineInstructionTag {
    // Beacuse we only have 32-bits interger and float in SysY, so we don't need
    // 64-bits-wise instruction provided by RV64GC

    PHI,

    // RV64I
    //// Integer Computation
    ADDIW,
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
    SLT,
    SLTI,
    SLTU,
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
    JAL,
    JALR,
    CALL, // presudo instruction
    J, // presudo instruction
    RET, // a user defined preduo instruction which should be replaced in prelude_conlusion pass
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
  string name;
  MachineInstructionTag tag;
  unique_ptr<vector<Register *>> oprands;
  unique_ptr<vector<MachineBasicBlock *>> j_targets;
  MachineFunction* fun;
  unique_ptr<Immediate> imm;
  MachineGlobal *global; // for load golbal
  Register* target;
  // in normal address mode like 100(a), a is in oprands[0], 100 is in imm

public:
  MachineInstruction(MachineInstructionTag tag, string name)
      : VRegister(name), tag(tag) {
    oprands = unique_ptr<vector<Register *>>(new vector<Register *>());
    j_targets = unique_ptr<vector<MachineBasicBlock *>>(
        new vector<MachineBasicBlock *>());
  }
  MachineInstruction(MachineInstructionTag tag) : VRegister(), tag(tag) {
    oprands = unique_ptr<vector<Register *>>(new vector<Register *>());
    j_targets = unique_ptr<vector<MachineBasicBlock *>>(
        new vector<MachineBasicBlock *>());
  }
  void replaceIRRegister(map<Instruction *, Register *> instr_map) {
    for (auto &opd : *oprands) {
      if (IRRegister *irr = dynamic_cast<IRRegister *>(opd)) {
        // 在映射中查找这个 Instruction 指针
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

  void pushReg(Register *r);
  void pushJTarget(MachineBasicBlock *b);
  void setImm(Immediate i);
  void setGlobal(MachineGlobal *global);
  void setTarget(Register* reg);
  void setFunction(MachineFunction *func);
  MachineInstructionTag getTag() const;
  Register *getReg(int idx) const;
  MachineBasicBlock *getJTarget(int idx) const;
  Immediate *getImm() const;
  MachineGlobal *getGlobal() const;
  Register* getTarget(Register* reg);
  MachineFunction *getFunction() const;
  void printASM(ostream &stream) const;
  bool is_float() const override {
    switch (tag) {
    case FADD_S:
    case FSUB_S:
    case FMUL_S:
    case FDIV_S:
    case FSQRT_S:
    case FMIN_S:
    case FMAX_S:
    case FNMADD_S:
    case FNMSUB_S:
    case FMV_S_X:
    case FMV_X_S:
    case FLW:
    case FSW:
    case FCVTS_W:
    case FCVTS_WU:
    case FCVTW_S:
    case FCVTWU_S:
    case FEQ_S:
    case FLT_S:
    case FLE_S:
      return true;
    default:
      return false; // 所有未列出的枚举值都返回 false
    }
  }
};

#endif