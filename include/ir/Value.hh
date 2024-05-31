/**
 * Value: base class of ir
 * Create by Zhang Junbin at 2024/5/31
 */

#ifndef _VALUE_H_
#define _VALUE_H_

#include <string>

#include "Type.hh"
#include "Use.hh"

using std::string;

enum ValueTag {
  VT_VALUE,
  VT_INSTR,
  VT_ALLOCA,
  VT_BOP,
  VT_BR,
  VT_CALL,
  VT_CMP,
  VT_FPTOSI,
  VT_GEP,
  VT_JUMP,
  VT_LOAD,
  VT_PHI,
  VT_RET,
  VT_SITOFP,
  VT_STORE,
  VT_UOP,
  VT_BB,
  VT_CONSTANT,
  VT_INTCONST,
  VT_FLOATCONST,
  VT_ARRCONST,
  VT_GLOBALVAL,
  VT_GLOBALVAR,
  VT_FUNC,
  VT_ARG,
};

class Value {
 private:
  string vName;
  Type* vType{nullptr};
  // Record subclass types
  ValueTag vTag{VT_VALUE};
  Use* useHead;

 public:
  Value(){};
  Value(Type* vt, ValueTag vtag) : vType(vt), vTag(vtag){};
  Value(Type* vt, string vn, ValueTag vtag)
      : vType(vt), vName(vn), vTag(vtag){};

  virtual void printIR() const {};
  string getName() const { return vName; }
  ValueTag getValueTag() const { return vTag; }
  Type* getType() const { return vType; }
};

#endif