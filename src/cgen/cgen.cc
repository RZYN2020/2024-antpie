#include "cgen.hh"
#include <fstream>
#include <iostream>

void generate_code(MModule *res, ANTPIE::Module *ir) {
  select_instruction(res, ir);

  std::ofstream out_s;
  out_s.open("tests/test.phi.s");
  out_s << *res;

  allocate_register(res);
}