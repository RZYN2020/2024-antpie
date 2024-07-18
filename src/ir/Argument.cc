#include
#include "ir/Type.hh"
#include "ir/Argument.hh"

string Argument::toString() const {
  return "%" + getName();
}
