#include "cgen.hh"
#include <fstream>
#include <iostream>

void generate_code(MModule *res, ANTPIE::Module *ir, bool opt) {
  select_instruction(res, ir);

  std::ofstream out_s;
  out_s.open("tests/test.phi.s");
  out_s << *res;

  if (opt) {
    allocate_register(res);
  } else {
    spill_all_register(res);
  }
}