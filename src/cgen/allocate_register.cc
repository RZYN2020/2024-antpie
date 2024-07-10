#include "Machine.hh"
#include <set>

using std::set;

// void ssa_liveness(MFunction* func, map<MInstruction*, vector<Register*>> liveness) {
//   for (auto r: func->getVariables()) {
//     vector<MBasicBlock*> scanned_blocks = {};
//     for (auto ins : r->getUses()){
//       if (ins == r) continue; // def r
//       if (ins->getInsTag() == MInstruction::H_PHI){
        
//       } else {
//         scan_liveIn(ins, r);
//       }
//     }
//   }
// }

// void scan_liveIn(MInstruction* ins, Register* r) {
//   ///
// }

// void scan_liveOut(MInstruction* ins, Register* r) {
//   ///
// }

// Register Allocation on SSA Form
void allocate_register(MModule *mod) {
  for (auto &func : mod->getFunctions()) {
  // step1. Liveness Analysis
  auto liveness_ireg = make_unique<map<MInstruction*, vector<Register*>>>();
  auto liveness_freg = make_unique<map<MInstruction*, vector<Register*>>>();

  // step2. Spill
  auto arg_addr = make_unique<map<Register*, int>>();
  int offset = 0; // from sp, upward->minus
  auto spills_ireg = make_unique<map<Register*, int>>(); // offset without considering saved registers
  auto spills_preg = make_unique<map<Register*, int>>(); // pointer register should be 64-bit wise
  auto spills_freg = make_unique<map<Register*, int>>(); 

  // step3. Allocate
  auto used_regster = make_unique<set<Register*>>(); // which physical register all used
  auto allocation_ireg = make_unique<map<Register*, IRegister*>>();
  auto allocation_freg = make_unique<map<Register*, FRegister*>>();

  // step4. Rewrite program
  // 4.1 Remove Phi -> push move instruction to the end of the bb
  // 4.2 Save used register at the entry of the function
  // 4.4 Spill Registers
  // 4.5 Allocate Registers
  // 4.6 lower alloca
  // 4.7 Handle arguments passing (Argument in Reg) -> (Allocated Addr)
  // 4.8 Lower call instruction
  // + push store callee saved registes
  // + move arguments -> push arguments
  // + call
  // + pop callee saved registers
  // 4.9 Restore saved register at the exit of the function
  }
}