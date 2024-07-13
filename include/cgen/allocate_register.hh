#pragma once
#include "Machine.hh"
#include "Module.hh"
#include <set>
using std::pair;
using std::set;

using LivenessInfo = map<MInstruction *, set<Register *>>;
std::vector<Register *> getUsesV_FRegister(MInstruction *ins);
std::vector<Register *> getUsesV_IRegister(MInstruction *ins);
std::vector<Register *> getDefsV_FRegister(MInstruction *ins);
std::vector<Register *> getDefsV_IRegister(MInstruction *ins);
std::vector<Register *> getPhiDefsV_FRegister(MBasicBlock *ins);
std::vector<Register *> getPhiDefsV_IRegister(MBasicBlock *ins);

void printLivenessInfo(MFunction *func, LivenessInfo *liveness_ireg,
                       LivenessInfo *liveness_freg);

void out_of_ssa(MFunction *func);
void rewrite_program_spill(MFunction *func, map<Register *, int> *spill);
void rewrite_program_allocate(MFunction *func,
                              map<Register *, Register *> *allocation);
void lower_alloca(MFunction *func, int &stack_offset);
void add_prelude(MFunction *func,
                        map<Register *, Register *> *allocation,
                        map<Register *, int> *spill, int stack_offset,
                        set<Register *> *callee_saved);
void add_conclude(MFunction *func,
                         map<Register *, Register *> *allocation,
                         map<Register *, int> *spill, int stack_offset,
                         set<Register *> *callee_saved);

#define MAX_I_REG_NUM 23 // without t0 t1 t2, zero ra sp gp tp s0
#define MAX_F_REG_NUM 29 // without ft0 ft1 ft2

Register *getOneFRegiter(set<Register *> *used);
Register *getOneIRegiter(set<Register *> *used);
set<Register *> getActuallCalleeSavedRegisters(map<Register *, Register *> *allocation);
set<Register *> getActuallCallerSavedRegisters(map<Register *, Register *> *allocation, set<Register *> liveInOfcallsite);

// https://stackoverflow.com/questions/3407012/rounding-up-to-the-nearest-multiple-of-a-number
static int roundUp(int numToRound, int multiple) {
  assert(multiple && ((multiple & (multiple - 1)) == 0));
  return (numToRound + multiple - 1) & -multiple;
}
