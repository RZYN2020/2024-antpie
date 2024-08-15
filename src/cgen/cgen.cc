#include "cgen.hh"

#include <fstream>
#include <iostream>

void generate_code(MModule *res, ANTPIE::Module *ir, bool opt) {
  select_instruction(res, ir);
  // std::ofstream out_s;
  // out_s.open("tests/test.phi.s");
  // out_s << *res;
  schedule_instruction(res);
  allocate_register(res);
  peephole_optimize(res);
  branch_simplify(res);
}