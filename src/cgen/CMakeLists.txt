add_library(
    cgen
    cgen.cc
    select_instruction.cc
    allocate_register.cc
    spill_allocation.cc
    saa_allocation.cc
)