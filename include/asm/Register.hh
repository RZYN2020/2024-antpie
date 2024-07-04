#ifndef _REGISTER_H_
#define _REGISTER_H_

#include "Instruction.hh"
#include "Type.hh"
#include <iostream>
#include <string>

using std::ostream;
using std::string;

class MachineInstruction;
class IRegister;
class FRegister;

class Register {

private:
  std::vector<MachineInstruction *> uses;


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
    IR_REGISTER, // to be resolved
  };

protected:
  RegTag tag;
  int id;
  string name;

public:
  virtual string getName() const = 0;
  virtual bool is_float() const = 0;
  RegTag getTag() { return tag; }

  void addUse(MachineInstruction *use) { uses.push_back(use); }
  virtual void replaceRegisterUsers(Register *newReg) = 0;

  void removeUse(MachineInstruction *use) {
    for (auto it = uses.begin(); it != uses.end(); ++it) {
      if (*it == use) {
        uses.erase(it);
        break;
      }
    }
  }

  std::vector<MachineInstruction *> getUses() { return uses; }
  void clearUses() { uses.clear(); }
};

class IRRegister : public Register {
public:
  Instruction *ir_reg;
  IRRegister(Instruction *ins) {
    tag = IR_REGISTER;
    ir_reg = ins;
  };
  string getName() const override { return ir_reg->getName(); }
  bool is_float() const override {
    return ir_reg->getType() == FloatType::getFloatType();
  }
  void replaceRegisterUsers(Register *newReg) override {}
};

static int get_id() {
  static int cnt = 0;
  return cnt++;
}

class VRegister : public Register {
public:
  VRegister() {
    tag = V_REGISTER;
    id = get_id();
    name = "t" + std::to_string(id);
  }

  VRegister(string n) {
    tag = V_REGISTER;
    id = get_id();
    name = n;
  }

  void setName(string name_) { name = name_; }

  string getName() const override { return name; }
};

class IRegister : public Register {
public:
  IRegister(int id_, std::string n) {
    tag = I_REGISTER;
    id = id_;
    name = n;
  }
  string getName() const override { return name; }
  bool is_float() const override { return false; }
  void replaceRegisterUsers(Register *newReg) override {}
};

class FRegister : public Register {
public:
  FRegister(int id_, std::string n) {
    tag = F_REGISTER;
    id = id_;
    name = n;
  }
  string getName() const override { return name; }
  bool is_float() const override { return true; }
  void replaceRegisterUsers(Register *newReg) override {}
};
#endif