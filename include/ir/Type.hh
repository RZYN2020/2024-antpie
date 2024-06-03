/**
 * Type: type of value
 * Create by Zhang Junbin at 2024/5/31
 */

#ifndef _TYPE_H_
#define _TYPE_H_
#include <memory>
#include <vector>

#include "Argument.hh"
using std::make_unique;
using std::unique_ptr;
using std::vector;

enum TypeTag {
  TT_VOID,
  TT_INT1,
  TT_INT32,
  TT_FLOAT,
  TT_ARRAY,
  TT_FUNCTION,
  TT_POINTER
};

class VoidType;
class Int1Type;
class Int32Type;
class FloatType;
class ArrayType;
class FuncType;

class Type {
 private:
  TypeTag tTag;

  // Singleton
  static VoidType* voidType;
  static Int1Type* int1Type;
  static Int32Type* int32Type;
  static FloatType* floatType;

 public:
  Type(TypeTag tt) : tTag(tt) {}
  virtual string toString() const { return ""; }
  static inline Int1Type* getInt1Type() { return int1Type; }
  static inline Int32Type* getInt32Type() { return int32Type; }
  static inline FloatType* getFloatType() { return floatType; }
  static inline VoidType* getVoidType() { return voidType; }
  TypeTag getTypeTag() const { return tTag; }
};

class VoidType : public Type {
 public:
  string toString() const override;
  VoidType() : Type(TT_VOID) {}
};

class Int1Type : public Type {
 public:
  string toString() const override;
  Int1Type() : Type(TT_INT1) {}
};

class Int32Type : public Type {
 public:
  string toString() const override;
  Int32Type() : Type(TT_INT32) {}
};

class FloatType : public Type {
 public:
  string toString() const override;
  FloatType() : Type(TT_FLOAT) {}
};

class ArrayType : public Type {
 private:
  int len;
  Type* elemType;

 public:
  string toString() const override;
  ArrayType(int l, Type* eT) : Type(TT_ARRAY), len(l), elemType(eT) {}
  int getLen() const { return len; }
  Type* getElemType() const { return elemType; }
};

class FuncType : public Type {
 private:
  Type* retType;
  unique_ptr<vector<unique_ptr<Argument>>> arguments;

 public:
  FuncType(Type* rType);
  void pushArgument(Argument* arg);
  Type* getRetType() const { return retType; }
  int getArgSize() const { return arguments->size(); }
  Argument* getArgument(int idx) { return arguments->at(idx).get(); }
};

class PointerType : public Type {
 private:
  Type* elemType;

 public:
  string toString() const override;
  PointerType(Type* et) : Type(TT_POINTER), elemType(et) {}
  Type* getElemType() const { return elemType; }
};

#endif