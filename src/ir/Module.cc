#include "ir/Module.hh"

using ANTPIE::Module;

Module::Module() {
  globalVariables = make_unique<vector<unique_ptr<GlobalVariable>>>();
  functions = make_unique<vector<unique_ptr<Function>>>();
}

void Module::pushFunction(Function* function) {
  functions->push_back(unique_ptr<Function>(function));
}

void Module::pushGlobalVariable(GlobalVariable* globalVariable) {
  globalVariables->push_back(unique_ptr<GlobalVariable>(globalVariable));
}

void Module::printIR(ostream& stream) const {
  for (const auto& gv : *globalVariables) {
    gv->printIR(stream);
    stream << endl;
  }
  stream << endl;
  for (const auto& f : *functions) {
    f->printIR(stream);
    stream << endl;
  }
}