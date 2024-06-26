#include "Constant.hh"

map<int, IntegerConstant*> IntegerConstant::constBuffer;
map<float, FloatConstant*> FloatConstant::constBuffer;

IntegerConstant* IntegerConstant::getConstInt(int num) {
  auto elem = constBuffer.find(num);
  if (elem != constBuffer.end()) {
    return elem->second;
  }
  IntegerConstant* intConst = new IntegerConstant(num);
  constBuffer.emplace(num, intConst);
  return intConst;
}

FloatConstant* FloatConstant::getConstFloat(float num) {
  auto elem = constBuffer.find(num);
  if (elem != constBuffer.end()) {
    return elem->second;
  }
  FloatConstant* floatConst = new FloatConstant(num);
  constBuffer.emplace(num, floatConst);
  return floatConst;
}

Constant* Constant::getZeroConstant(Type* t) {
  switch (t->getTypeTag()) {
    case TT_INT1:
    case TT_INT32:
      return new IntegerConstant();
    case TT_FLOAT:
      return new FloatConstant();
    case TT_ARRAY:
      return new ArrayConstant(t);
    default:
      return nullptr;
  }
}

string ArrayConstant::toString() const {
  if (isZeroInit()) {
    return "zeroinitializer";
  }
  string arrStr = "[";
  int len = ((ArrayType*)getType())->getLen();
  Type* type = ((ArrayType*)getType())->getElemType();
  for (int i = 0; i < len; i++) {
    auto loc = elems->find(i);
    Constant* elem;
    if (loc != elems->end()) {
      elem = loc->second;
    } else {
      elem = Constant::getZeroConstant(type);
    }
    if (i != 0) {
      arrStr += ", ";
    }
    arrStr += type->toString();
    arrStr += " ";
    arrStr += elem->toString();
  }
  arrStr += "]";
  return arrStr;
}

void ArrayConstant::put(int loc, Constant* v) { 
  setZeroInit(false);
  elems->emplace(loc, v); }