#ifndef _MACHINE_H_
#define _MACHINE_H_

#include "Constant.hh"
#include "GlobalVariable.hh"
#include "Type.hh"
#include "MachineInstruction.hh"

class MachineBasicBlock {
private:
  string name;
  unique_ptr<vector<unique_ptr<MachineInstruction>>> instructions;
  MachineFunction *function;

public:
  MachineBasicBlock(string name_);
  void pushInstr(MachineInstruction *i);
  void pushInstrs(vector<MachineInstruction *> is);
  void pushInstrBeforeJmp(MachineInstruction *i);
  void pushInstrAtHead(MachineInstruction *i);
  void pushInstrsAtHead(vector<MachineInstruction *> is);
  string getName() const { return "." + name; }
  string to_string() const;
  void setFunction(MachineFunction *function);
  MachineFunction *getFunction();

  unique_ptr<MachineInstruction> removeInstruction(MachineInstruction *ins);

  void replaceInstructionWith(MachineInstruction *ins,
                              vector<MachineInstruction *> instrs);
  void insertBeforeInstructionWith(MachineInstruction *ins,
                                   vector<MachineInstruction *> instrs);

  void insertAfterInstructionWith(MachineInstruction *ins,
                                  vector<MachineInstruction *> instrs);

  const vector<unique_ptr<MachineInstruction>> &getInstructions() const {
    return *instructions;
  }
};

uint32_t cal_size(const Type *tp);

class MachineGlobal {
private:
  GlobalVariable *global;

public:
  MachineGlobal(GlobalVariable *global) : global(global) {}
  string to_string() const;
  string getName() const { return global->getName(); }
};

class MachineFunction {
private:
  FuncType *type;
  string name;
  unique_ptr<vector<unique_ptr<MachineBasicBlock>>> basicBlocks;
  unique_ptr<vector<Register *>> saved_registers;
  uint32_t spilled_size =
      0; // Without the user directly using alloca in the source code, we can
         // statically determine the spilled size.
  unique_ptr<vector<unique_ptr<MachineInstruction>>> reg_pool;

public:
  MachineFunction(FuncType *fType, string name);
  void pushBasicBlock(MachineBasicBlock *bb);
  string to_string() const;
  string getName() const { return name; }
  uint32_t getSavedSize() const {
    return saved_registers->size() * 4 + 2 * 8; // with ra, fp
  }
  uint32_t getSpilledSize() const { return spilled_size; }
  void incSpilledSize(uint32_t sz) { spilled_size += sz; }

  const vector<unique_ptr<MachineBasicBlock>> &getBasicBlocks() const {
    return *basicBlocks;
  }

  void pushIntoRegPool(MachineInstruction *reg) {
    reg_pool->push_back(unique_ptr<MachineInstruction>(reg));
  }

  const vector<unique_ptr<MachineInstruction>> &getRegPool() const {
    return *reg_pool;
  }
};

class MachineModule {
private:
  unique_ptr<vector<unique_ptr<MachineGlobal>>> globalVariables;
  unique_ptr<vector<unique_ptr<MachineFunction>>> functions;
  MachineBasicBlock *currBasicBlock;

public:
  MachineModule();
  string to_string() const;
  void setCurrBasicBlock(MachineBasicBlock *bb) { currBasicBlock = bb; }
  MachineFunction *addFunction(FuncType *funcType, string name);
  MachineBasicBlock *addBasicBlock(MachineFunction *function, string name);

  MachineGlobal *addGlobalVariable(GlobalVariable *global);
  MachineGlobal *addGlobalFloat(FloatConstant *f);

  const vector<unique_ptr<MachineGlobal>> &getGlobals() const {
    return *globalVariables;
  }

  const vector<unique_ptr<MachineFunction>> &getFunctions() const {
    return *functions;
  }
};

#endif