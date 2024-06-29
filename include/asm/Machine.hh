#ifndef _MACHINE_H_
#define _MACHINE_H_

#include "Constant.hh"
#include "GlobalVariable.hh"
#include "MachineInstruction.hh"
#include "Type.hh"


class MachineFunction {
private:
  FuncType *type;
  string name;
  unique_ptr<vector<unique_ptr<MachineBasicBlock>>> basicBlocks;

public:
  MachineFunction(FuncType *fType, string name);
  void pushBasicBlock(MachineBasicBlock *bb);
  void printASM(ostream &stream) const;

  const vector<unique_ptr<MachineBasicBlock>>& getBasicBlocks() const {
    return *basicBlocks;
  }
};

class MachineModule {
private:
  unique_ptr<vector<unique_ptr<MachineGlobal>>> globalVariables;
  unique_ptr<vector<unique_ptr<MachineFunction>>> functions;
  MachineBasicBlock *currBasicBlock;

public:
  MachineModule();
  void printASM(ostream &stream) const;
  void setCurrBasicBlock(MachineBasicBlock *bb) { currBasicBlock = bb; }
  MachineFunction *addFunction(FuncType *funcType, string name);
  MachineBasicBlock *addBasicBlock(MachineFunction *function, string name);

  MachineGlobal *addGlobalVariable(GlobalVariable *global);
  MachineGlobal *addGlobalFloat(FloatConstant *f);

  const vector<unique_ptr<MachineGlobal>>& getGlobals() const {
    return *globalVariables;
  }

  const vector<unique_ptr<MachineFunction>>& getFunctions() const {
    return *functions;
  }
};

#endif