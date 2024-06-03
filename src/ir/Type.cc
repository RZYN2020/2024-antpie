#include "ir/Type.hh"

Int1Type* Type::int1Type = new Int1Type();
Int32Type* Type::int32Type = new Int32Type();
FloatType* Type::floatType = new FloatType();
VoidType* Type::voidType = new VoidType();

string VoidType::toString() const { return "void"; }

string Int1Type::toString() const { return "i1"; }

string Int32Type::toString() const { return "i32"; }

string FloatType::toString() const { return "float"; }

string ArrayType::toString() const {
  return "[" + std::to_string(len) + " x " + elemType->toString() + "]";
}

string PointerType::toString() const { return elemType->toString() + "*"; }

FuncType::FuncType(Type* rType) : Type(TT_FUNCTION), retType(rType) {
  arguments = make_unique<vector<unique_ptr<Argument>>>();
};

void FuncType::pushArgument(Argument* arg) {
  arguments->push_back(unique_ptr<Argument>(arg));
}
