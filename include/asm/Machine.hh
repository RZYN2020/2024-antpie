#ifndef _MACHINE_H_
#define _MACHINE_H_

#include "Constant.hh"
#include "GlobalVariable.hh"
#include "MachineInstruction.hh"
#include "Type.hh"

class MachineBasicBlock {
private:
  string name;
  unique_ptr<vector<unique_ptr<MachineInstruction>>> instructions;

public:
  MachineBasicBlock(string name_) {
    name = name_;
    instructions = make_unique<vector<unique_ptr<MachineInstruction>>>();
  }
  void pushInstr(MachineInstruction *i) {
    instructions->push_back(unique_ptr<MachineInstruction>(i));
  }

  void pushInstrs(vector<MachineInstruction *> is) {
    for (auto i : is) {
      instructions->push_back(unique_ptr<MachineInstruction>(i));
    }
  }

  string getName() const { return "." + name; }

  string to_string() const;

  const vector<unique_ptr<MachineInstruction>> &getInstructions() const {
    return *instructions;
  }

  unique_ptr<MachineInstruction> removeInstruction(MachineInstruction *ins) {
    for (auto it = instructions->begin(); it != instructions->end();) {
      if (it->get() == ins) {
        unique_ptr<MachineInstruction> removed = std::move(*it);
        it = instructions->erase(it);
        return removed;
      } else {
        ++it;
      }
    }
    return nullptr;
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

public:
  MachineFunction(FuncType *fType, string name);
  void pushBasicBlock(MachineBasicBlock *bb);
  string to_string() const;
  string getName() const { return name; }

  const vector<unique_ptr<MachineBasicBlock>> &getBasicBlocks() const {
    return *basicBlocks;
  }
};

class MachineModule {
private:
  unique_ptr<vector<unique_ptr<MachineGlobal>>> globalVariables;
  unique_ptr<vector<unique_ptr<MachineFunction>>> functions;
  MachineBasicBlock *currBasicBlock;
  unique_ptr<vector<unique_ptr<VRegister>>> reg_pool;

public:
  MachineModule();
  string to_string() const;
  void setCurrBasicBlock(MachineBasicBlock *bb) { currBasicBlock = bb; }
  MachineFunction *addFunction(FuncType *funcType, string name);
  MachineBasicBlock *addBasicBlock(MachineFunction *function, string name);

  MachineGlobal *addGlobalVariable(GlobalVariable *global);
  MachineGlobal *addGlobalFloat(FloatConstant *f);

  void pushIntoRegPool(VRegister *reg) {
    reg_pool->push_back(unique_ptr<VRegister>(reg));
  }

  const vector<unique_ptr<MachineGlobal>> &getGlobals() const {
    return *globalVariables;
  }

  const vector<unique_ptr<MachineFunction>> &getFunctions() const {
    return *functions;
  }
};

#endif