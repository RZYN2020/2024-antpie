/**
 * GlobalVariable ir
 * Create by Zhang Junbin at 2024/6/2
 */
#ifndef _GLOBAL_VARIABLE_H_
#define _GLOBAL_VARIABLE_H_

#include "Constant.hh"

class GlobalVariable : public GlobalValue {
 public:
  // zeroinit
  GlobalVariable(Type* type, string name);
  GlobalVariable(Type* type, Constant* initValue, string name);
};

#endif