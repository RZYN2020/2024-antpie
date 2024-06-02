#include "../../include/ir/Function.hh"

// define dso_local i32 @foo(i32 %a, float %b) {
// entry:
//   ret i32 1
// }
void Function::printIR(ostream& stream) const {
  FuncType* funcType = dynamic_cast<FuncType*>(getType());
  stream << "define dso_local " << funcType->getRetType()->toString() << " @"
         << getName() << "(";
  int argSize = funcType->getArgSize();
  for (int i = 0; i < argSize; i++) {
    if (i != 0) {
      stream << ", ";
    }
    stream << funcType->getArgument(i)->toString();
  }
  stream << ") {";
  for (const auto& bb : *basicBlocks) {
    bb->printIR(stream);
    stream << endl;
  }
  stream << "}" << endl;
}