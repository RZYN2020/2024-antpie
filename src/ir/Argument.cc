#include "ir/Argument.hh"
#include "ir/Type.hh"

string Argument::toString() const {
  return "%" + getName();
}
