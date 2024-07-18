#include <fstream>
#include <iostream>

#include "BasicBlock.hh"
#include "Constant.hh"
#include "DomTree.hh"
#include "Function.hh"
#include "Instruction.hh"
#include "Machine.hh"
#include "Module.hh"
#include "MySysYParserVisitor.h"
#include "SysYParserLexer.h"
#include "SysYParserParser.h"
#include "antlr4-runtime.h"
#include "cgen.hh"


int main() {

  std::ifstream source("./tests/test.c");
  assert(source);

  antlr4::ANTLRInputStream input(source);

  SysYParserLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);

  SysYParserParser parser(&tokens);

  auto visitor = new MySysYParserVisitor(variableTable(nullptr));

  auto prgc= parser.program();

  std::cout << "helloccc!\n";

  visitor->visitProgram(prgc);

  std::cout << "hello!\n";

  ANTPIE::Module *module = &visitor->module;
  std::ofstream out_ll;
  out_ll.open("tests/test.ll");
  module->printIR(out_ll);


  module->irOptimize();

  std::ofstream opt_ll;
  out_ll.open("tests/test.opt.ll");
  module->printIR(opt_ll);

  MModule *mmodule = new MModule();
  generate_code(mmodule, module);
  std::ofstream out_s;
  out_s.open("tests/test.s");
  out_s << *mmodule;
}