#include "cgen.hh"


int getDelay(MInstruction* ins) {
    
}


void schedule(MBasicBlock *bb) {

  vector<MInstruction *> instrs; // todo: get all instrs with ownership
  vector<MInstruction *> ready;

  // step1. Build dependence graph
  // node: instr // edge: def->use
  // actually no need to build

  // step2. Assign priorities to each operation
  // travel...

  // step3. Iteratively select an operation and schedule it
  // select...

  int cycle = 1;
  vector<MInstruction *> active; // active may be a more efficient datastructure...
  while (ready.size() != 0 && active.size() != 0) {
    // for each op ∈ Active
    for (auto ins : active) {
        // if S(op) + delay(op) < Cycle then
        // remove op from Active
        // for each successor s of op in D
            // if s is ready
                // then add s to Ready
    }

    // if Ready ̸= ∅ then
        // remove an op from Ready
        // S(op) ← Cycle
        // add op to Active
    cycle++;
  }
}

// see https://starfivetech.com/uploads/u74_core_complex_manual_21G1.pdf for
// instruction delays
void schedule_instruction(MModule *mod) {
  for (auto func : mod->getFunctions()) {
    for (auto bb : func->getBasicBlocks()) {
      schedule(bb);
    }
  }
}