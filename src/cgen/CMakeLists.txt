add_library(
    cgen
    cgen.cc
    select_instruction.cc
    schedule_instruction.cc
    allocate_register.cc
    spill_allocation.cc
    ssa_allocation.cc
    peephole_optimization.cc
    branch_simplification.cc
)