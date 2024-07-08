#include "cgen.hh"

void generate_code(MModule* res, ANTPIE::Module* ir) {
    select_instruction(res, ir);
    allocate_register(res);
    prelude_conclusion(res);
}