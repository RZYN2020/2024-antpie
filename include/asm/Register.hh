#ifndef _REGISTER_H_
#define _REGISTER_H_

#include "Instruction.hh"
#include "Type.hh"
#include <iostream>
#include <string>

using std::ostream;
using std::string;

class MachineInstruction;

class Register {

private:
  std::vector<MachineInstruction *> uses;

public:
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
  void clearUses() {uses.clear();}
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

#define CONCAT(x, y) x##y
#define DEFINE_IREGISTER(regName, regId)                                       \
  static IRegister CONCAT(reg_, regName)((regId), #regName)
#define DEFINE_FREGISTER(regName, regId)                                       \
  static FRegister CONCAT(reg_, regName)((regId), #regName)

DEFINE_IREGISTER(zero, 0);
DEFINE_IREGISTER(ra, 1); // saved
DEFINE_IREGISTER(sp, 2); // saved
DEFINE_IREGISTER(gp, 3);
DEFINE_IREGISTER(tp, 4);
DEFINE_IREGISTER(t0, 5);
DEFINE_IREGISTER(t1, 6);
DEFINE_IREGISTER(t2, 7);
DEFINE_IREGISTER(s0, 8); // 注意：s0 和 fp 是同一个寄存器
DEFINE_IREGISTER(s1, 9);
DEFINE_IREGISTER(a0, 10);
DEFINE_IREGISTER(a1, 11);
DEFINE_IREGISTER(a2, 12);
DEFINE_IREGISTER(a3, 13);
DEFINE_IREGISTER(a4, 14);
DEFINE_IREGISTER(a5, 15);
DEFINE_IREGISTER(a6, 16);
DEFINE_IREGISTER(a7, 17);
DEFINE_IREGISTER(s2, 18);
DEFINE_IREGISTER(s3, 19);
DEFINE_IREGISTER(s4, 20);
DEFINE_IREGISTER(s5, 21);
DEFINE_IREGISTER(s6, 22);
DEFINE_IREGISTER(s7, 23);
DEFINE_IREGISTER(s8, 24);
DEFINE_IREGISTER(s9, 25);
DEFINE_IREGISTER(s10, 26);
DEFINE_IREGISTER(s11, 27);
DEFINE_IREGISTER(t3, 28);
DEFINE_IREGISTER(t4, 29);
DEFINE_IREGISTER(t5, 30);
DEFINE_IREGISTER(t6, 31);

static IRegister *iregisters[] = {
    &CONCAT(reg_, zero), &CONCAT(reg_, ra), &CONCAT(reg_, sp),
    &CONCAT(reg_, gp),   &CONCAT(reg_, tp), &CONCAT(reg_, t0),
    &CONCAT(reg_, t1),   &CONCAT(reg_, t2), &CONCAT(reg_, s0),
    &CONCAT(reg_, s1),   &CONCAT(reg_, a0), &CONCAT(reg_, a1),
    &CONCAT(reg_, a2),   &CONCAT(reg_, a3), &CONCAT(reg_, a4),
    &CONCAT(reg_, a5),   &CONCAT(reg_, a6), &CONCAT(reg_, a7),
    &CONCAT(reg_, s2),   &CONCAT(reg_, s3), &CONCAT(reg_, s4),
    &CONCAT(reg_, s5),   &CONCAT(reg_, s6), &CONCAT(reg_, s7),
    &CONCAT(reg_, s8),   &CONCAT(reg_, s9), &CONCAT(reg_, s10),
    &CONCAT(reg_, s11),  &CONCAT(reg_, t3), &CONCAT(reg_, t4),
    &CONCAT(reg_, t5),   &CONCAT(reg_, t6),
};

static Register *getIRegister(int idx) {
  if (idx < 0 || idx >= (sizeof(iregisters) / sizeof(iregisters[0]))) {
    throw std::out_of_range("Index is out of the range of IRegister array.");
  }
  return iregisters[idx];
}

DEFINE_FREGISTER(ft0, 0);
DEFINE_FREGISTER(ft1, 1);
DEFINE_FREGISTER(ft2, 2);
DEFINE_FREGISTER(ft3, 3);
DEFINE_FREGISTER(ft4, 4);
DEFINE_FREGISTER(ft5, 5);
DEFINE_FREGISTER(ft6, 6);
DEFINE_FREGISTER(ft7, 7);
DEFINE_FREGISTER(fs0, 8);
DEFINE_FREGISTER(fs1, 9);
DEFINE_FREGISTER(fa0, 10);
DEFINE_FREGISTER(fa1, 11);
DEFINE_FREGISTER(fa2, 12);
DEFINE_FREGISTER(fa3, 13);
DEFINE_FREGISTER(fa4, 14);
DEFINE_FREGISTER(fa5, 15);
DEFINE_FREGISTER(fa6, 16);
DEFINE_FREGISTER(fa7, 17);
DEFINE_FREGISTER(fs2, 18);
DEFINE_FREGISTER(fs3, 19);
DEFINE_FREGISTER(fs4, 20);
DEFINE_FREGISTER(fs5, 21);
DEFINE_FREGISTER(fs6, 22);
DEFINE_FREGISTER(fs7, 23);
DEFINE_FREGISTER(fs8, 24);
DEFINE_FREGISTER(fs9, 25);
DEFINE_FREGISTER(fs10, 26);
DEFINE_FREGISTER(fs11, 27);
DEFINE_FREGISTER(ft8, 28);
DEFINE_FREGISTER(ft9, 29);
DEFINE_FREGISTER(ft10, 30);
DEFINE_FREGISTER(ft11, 31);

static FRegister *fregisters[] = {
    &CONCAT(reg_, ft0),  &CONCAT(reg_, ft1),  &CONCAT(reg_, ft2),
    &CONCAT(reg_, ft3),  &CONCAT(reg_, ft4),  &CONCAT(reg_, ft5),
    &CONCAT(reg_, ft6),  &CONCAT(reg_, ft7),  &CONCAT(reg_, fs0),
    &CONCAT(reg_, fs1),  &CONCAT(reg_, fa0),  &CONCAT(reg_, fa1),
    &CONCAT(reg_, fa2),  &CONCAT(reg_, fa3),  &CONCAT(reg_, fa4),
    &CONCAT(reg_, fa5),  &CONCAT(reg_, fa6),  &CONCAT(reg_, fa7),
    &CONCAT(reg_, fs2),  &CONCAT(reg_, fs3),  &CONCAT(reg_, fs4),
    &CONCAT(reg_, fs5),  &CONCAT(reg_, fs6),  &CONCAT(reg_, fs7),
    &CONCAT(reg_, fs8),  &CONCAT(reg_, fs9),  &CONCAT(reg_, fs10),
    &CONCAT(reg_, fs11), &CONCAT(reg_, ft8),  &CONCAT(reg_, ft9),
    &CONCAT(reg_, ft10), &CONCAT(reg_, ft11),
};

static Register *getFRegister(int idx) {
  if (idx < 0 || idx >= (sizeof(fregisters) / sizeof(fregisters[0]))) {
    throw std::out_of_range("Index is out of the range of FRegister array.");
  }
  return fregisters[idx];
}

class Immediate {
private:
  int32_t val;

public:
  Immediate(int32_t val) : val(val) {}
  string to_string() const { return std::to_string(val); }
};

#endif