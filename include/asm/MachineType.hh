#ifndef _MACHINE_TYPE_H_
#define _MACHINE_TYPE_H_

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///
///             ASM Data Types
///
///////////////////////////////////////////////////
///////////////////////////////////////////////////

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::string;
using std::unique_ptr;
using std::vector;
using std::make_unique;

enum MachineTypeTag {
  FLOAT,
  INT,
  ARRARY,
};

class MachineFloatType;
class MachineIntType;
class MachineArrayType;


class MachineType {
private:
  MachineTypeTag tag;
  static MachineFloatType *floatType;
  static MachineIntType *intType;

public:
  MachineType(MachineTypeTag tt) : tag(tt) {}
  static inline MachineFloatType *getMachineFloatType() { return floatType; }
  static inline MachineIntType *getMachineIntType() { return intType; }
  static MachineArrayType *getMachineArrayType(int n, MachineType *elemType);
  MachineTypeTag getMachineTypeTag() const { return tag; }
};

class MachineFloatType : public MachineType {
public:
  MachineFloatType() : MachineType(FLOAT) {}
};

class MachineIntType : public MachineType {
public:
  MachineIntType() : MachineType(INT) {}
};

class MachineArrayType : public MachineType {
private:
  int len;
  MachineType *elemType;

public:
  MachineArrayType(int l, MachineType *eT)
      : MachineType(ARRARY), len(l), elemType(eT) {}
  int getLen() const { return len; }
  MachineType *getElemType() const { return elemType; }
};


#endif