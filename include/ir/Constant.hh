/**
 * Constant ir
 * Create by Zhang Junbin at 2024/6/2
 */

#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <map>

#include "Value.hh"

using std::map;

class Constant : public Value {
 private:
  bool zeroInit;

 public:
  Constant(bool zI) : zeroInit(zI), Value(nullptr, VT_CONSTANT) {}
  Constant(bool zI, Type* t, ValueTag vt) : zeroInit(zI), Value(t, vt) {}
};

class IntegerConstant : public Constant {
 private:
  int value;

 public:
  IntegerConstant() : Constant(true, Type::getInt32Type(), VT_INTCONST) {}
  IntegerConstant(int v)
      : Constant(false, Type::getInt32Type(), VT_INTCONST), value(v) {}
  string toString() const override { return std::to_string(value); }
};

class FloatConstant : public Constant {
 private:
  int value;

 public:
  FloatConstant() : Constant(true, Type::getFloatType(), VT_FLOATCONST) {}
  FloatConstant(float v)
      : Constant(false, Type::getFloatType(), VT_FLOATCONST), value(v) {}
  string toString() const override { return std::to_string(value); }
};

class ArrayConstant : public Constant {
 private:
  unique_ptr<map<int, Constant*>> elems;

 public:
  ArrayConstant(Type* type) : Constant(true, type, VT_ARRCONST) {
    elems = make_unique<map<int, Constant*>>();
  }
  void put(int loc, Constant* v);
};

#endif