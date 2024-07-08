#pragma once
#include "Instruction.hh"
#include "Argument.hh"
#include <string>
#include <vector>

using std::string;
using std::vector;

class MInstruction;
class IRegister;
class FRegister;

class Register {
private:
  vector<MInstruction *> uses;

public:
  static IRegister *reg_zero;
  static IRegister *reg_ra; // saved
  static IRegister *reg_sp; // saved
  static IRegister *reg_gp;
  static IRegister *reg_tp;
  static IRegister *reg_t0;
  static IRegister *reg_t1;
  static IRegister *reg_t2;
  static IRegister *reg_s0; // 注意：s0 和 fp 是同一个寄存器
  static IRegister *reg_s1;
  static IRegister *reg_a0;
  static IRegister *reg_a1;
  static IRegister *reg_a2;
  static IRegister *reg_a3;
  static IRegister *reg_a4;
  static IRegister *reg_a5;
  static IRegister *reg_a6;
  static IRegister *reg_a7;
  static IRegister *reg_s2;
  static IRegister *reg_s3;
  static IRegister *reg_s4;
  static IRegister *reg_s5;
  static IRegister *reg_s6;
  static IRegister *reg_s7;
  static IRegister *reg_s8;
  static IRegister *reg_s9;
  static IRegister *reg_s10;
  static IRegister *reg_s11;
  static IRegister *reg_t3;
  static IRegister *reg_t4;
  static IRegister *reg_t5;
  static IRegister *reg_t6;

  static FRegister *reg_ft0;
  static FRegister *reg_ft1;
  static FRegister *reg_ft2;
  static FRegister *reg_ft3;
  static FRegister *reg_ft4;
  static FRegister *reg_ft5;
  static FRegister *reg_ft6;
  static FRegister *reg_ft7;
  static FRegister *reg_fs0;
  static FRegister *reg_fs1;
  static FRegister *reg_fa0;
  static FRegister *reg_fa1;
  static FRegister *reg_fa2;
  static FRegister *reg_fa3;
  static FRegister *reg_fa4;
  static FRegister *reg_fa5;
  static FRegister *reg_fa6;
  static FRegister *reg_fa7;
  static FRegister *reg_fs2;
  static FRegister *reg_fs3;
  static FRegister *reg_fs4;
  static FRegister *reg_fs5;
  static FRegister *reg_fs6;
  static FRegister *reg_fs7;
  static FRegister *reg_fs8;
  static FRegister *reg_fs9;
  static FRegister *reg_fs10;
  static FRegister *reg_fs11;
  static FRegister *reg_ft8;
  static FRegister *reg_ft9;
  static FRegister *reg_ft10;
  static FRegister *reg_ft11;

  static Register *getIRegister(int idx);

  static Register *getFRegister(int idx);

  enum RegTag {
    F_REGISTER,
    I_REGISTER,
    V_REGISTER,  // virtual_register
    A_REGISTER,  // argment register
    IR_REGISTER, // to be resolved
  };

private:
  RegTag tag;
  string name;

public:
  Register(string name, RegTag tag);
  string getName();
  void setName(string name);
  RegTag getTag();

  void addUse(MInstruction *use);
  void removeUse(MInstruction *use);
  void replaceRegisterWith(Register *newReg);
  vector<MInstruction *> &getUses();
  void clearUses();
};

class IRRegister : public Register {
public:
  Instruction *ir_reg;
  IRRegister(Instruction *ins);
};

class ARGRegister : public Register {
private:
  Argument *arg;

public:
  ARGRegister(Argument *arg);
};

class VRegister : public Register {
public:
  VRegister();
  VRegister(string name);
};

class IRegister : public Register {
private:
  int id;

public:
  IRegister(int id, string n);
};

class FRegister : public Register {
private:
  int id;

public:
  FRegister(int id, string n);
};
