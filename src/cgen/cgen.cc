#include "cgen.hh"

void generate_code(MModule* res, ANTPIE::Module* ir) {
    select_instruction(res, ir);
    spill_all_register(res);
}