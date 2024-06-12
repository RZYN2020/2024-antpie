#ifndef _MACHINE_H_
#define _MACHINE_H_

#include "MachineInstruction.hh"


///////////////////////////////////////////////////
///////////////////////////////////////////////////
///
///             ASM Program
///
///////////////////////////////////////////////////
///////////////////////////////////////////////////

#include "../ir/Constant.hh"

class MachineGlobal {
private:
  string name;
  MachineType type;
  Constant* init_val;
public:
  void printASM(ostream& stream) const;
};

class MachineBasicBlock {
 private:
  unique_ptr<vector<unique_ptr<MachineInstruction>>> instructions;
 public:
  MachineBasicBlock() {
    instructions = make_unique<vector<unique_ptr<MachineInstruction>>>();
  }
  void pushInstr(MachineInstruction* i) {
    instructions->push_back(unique_ptr<MachineInstruction>(i));
  }
  void printASM(ostream& stream) const;
};

class MachineFunction {
private:
  unique_ptr<vector<unique_ptr<MachineBasicBlock>>> basicBlocks;
 public:
  MachineFunction(FuncType* fType, string name);
  void pushBasicBlock(MachineBasicBlock* bb);
  void printASM(ostream& stream) const;
};


class MachineModule {
private:
  unique_ptr<vector<unique_ptr<MachineGlobal>>> globalVariables;
  unique_ptr<vector<unique_ptr<MachineFunction>>> functions;
  MachineBasicBlock *currBasicBlock;

public:
  MachineModule();
  void printASM(ostream& stream) const;
};

#endif