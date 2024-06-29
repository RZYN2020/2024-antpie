#ifndef _REGISTER_H_
#define _REGISTER_H_

#include <iostream>
#include <string>
#include "Instruction.hh"

using std::string;
using std::ostream;


class Register {

public:
  enum RegTag {
    F_REGISTER,
    I_REGISTER,
    V_REGISTER, // virtual_register
    IR_REGISTER, // to be resolved
  };

protected:
  RegTag tag;
  int id;
  string name;

public:
  virtual void print(ostream& stream) const = 0;
};

class IRRegister: public Register {
public:
  Instruction *ir_reg;
  IRRegister(Instruction* ins) {
    tag = IR_REGISTER;
    ir_reg = ins;
  };
    void print(ostream& stream) const override {
      stream << "name";
  }
};

static int get_id() {
    static int cnt = 0;
    return cnt++;
}

class VRegister: public Register {
public:

  VRegister() {
    tag = V_REGISTER;
    id = get_id();
    name = "t" + id;
  }

  VRegister(string n) {
    tag = V_REGISTER;
    id = get_id();
    name = n;
  }

  void print(ostream& stream) const override {
      stream << name;
  }
};

class IRegister : public Register {
public:
  IRegister(int id_, std::string n) {
    tag = I_REGISTER;
    id = id_;
    name = n;
  }
  void print(ostream& stream) const override {
      stream << name;
  }
};

class FRegister : public Register {
public:
  FRegister(int id_, std::string n) {
    tag = F_REGISTER;
    id = id_;
    name = n;
  }
  void print(ostream& stream) const override {
      stream << name;
  }
};

#define CONCAT(x, y) x##y
#define DEFINE_IREGISTER(regName, regId)                                       \
  static IRegister CONCAT(reg_, regName)((regId), #regName)
#define DEFINE_FREGISTER(regName, regId)                                       \
  static FRegister CONCAT(reg_, regName)((regId), #regName)

DEFINE_IREGISTER(zero, 0);
DEFINE_IREGISTER(ra, 1);
DEFINE_IREGISTER(sp, 2);
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

class Immediate {
// only int_32 imm in riscv
// allow f_32 imm in high level asm
private:
  uint32_t val;

public:
  Immediate(uint32_t val) : val(val) {}
};

#endif