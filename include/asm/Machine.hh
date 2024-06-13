#ifndef _MACHINE_H_
#define _MACHINE_H_

#include "MachineInstruction.hh"
#include "../ir/Type.hh"
#include "../ir/Constant.hh"
#include "../ir/GlobalVariable.hh"

class MachineGlobal {
private:
  GlobalVariable* global;
public:
  MachineGlobal(GlobalVariable* global) : global(global) {}
  void printASM(ostream& stream) const;
};

class MachineBasicBlock {
 private:
  string name;
  unique_ptr<vector<unique_ptr<MachineInstruction>>> instructions;
 public:
  MachineBasicBlock(string name_) {
    name = name_;
    instructions = make_unique<vector<unique_ptr<MachineInstruction>>>();
  }
  void pushInstr(MachineInstruction* i) {
    instructions->push_back(unique_ptr<MachineInstruction>(i));
  }
  void printASM(ostream& stream) const;
};

class MachineFunction {
private:
  FuncType* type;
  string name;
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
  void setCurrBasicBlock(MachineBasicBlock* bb) { currBasicBlock = bb; }
  MachineFunction* addFunction(FuncType* funcType, string name);
  MachineBasicBlock* addBasicBlock(MachineFunction* function, string name);

  MachineGlobal* addGlobalVariable(GlobalVariable* global);
};

#endif