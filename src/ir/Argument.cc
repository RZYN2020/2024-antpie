#include "Argument.hh"
#include "Type.hh"

string Argument::toString() const {
  return "%" + getName();
}
