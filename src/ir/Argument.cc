#include "../../include/ir/Argument.hh"
#include "../../include/ir/Type.hh"

string Argument::toString() const {
  return getType()->toString() + " " + getName();
}
