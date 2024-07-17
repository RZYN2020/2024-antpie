#pragma once

#include "Constant.hh"
#include "MInstruction.hh"
#include "Type.hh"

class MBasicBlock {
private:
  string name;
  unique_ptr<vector<unique_ptr<MHIphi>>> phis;
  unique_ptr<vector<unique_ptr<MInstruction>>> instructions;
  unique_ptr<vector<unique_ptr<MInstruction>>> jmps;
  MFunction *function;
  unique_ptr<vector<MBasicBlock *>> outgoing;
  unique_ptr<vector<MBasicBlock *>> incoming;

public:
  MBasicBlock(string name);

  void pushInstr(MInstruction *i);
  void pushInstrs(vector<MInstruction *> is);

  void pushInstrAtHead(MInstruction *i);
  void pushInstrsAtHead(vector<MInstruction *> is);

  string getName() const { return "." + name; }

  void setFunction(MFunction *function);
  MFunction *getFunction();

  void pushJmp(MInstruction *ins);
  int getJmpNum();
  MInstruction *getJmp(int idx);
  void clearJmps();

  void pushPhi(MHIphi *phi);
  vector<unique_ptr<MHIphi>> &getPhis();


  void removeIncoming(MBasicBlock *bb);
  void addIncoming(MBasicBlock* bb);
  void replaceOutgoing(MBasicBlock* oldbb, MBasicBlock* newbb);
  void replacePhiIncoming(MBasicBlock* oldbb, MBasicBlock* newbb);
  vector<MBasicBlock *> &getIncomings();
  vector<MBasicBlock *> &getOutgoings();

  unique_ptr<MInstruction> removeInstruction(MInstruction *ins);
  void replaceInstructionWith(MInstruction *ins, vector<MInstruction *> instrs);
  void insertBeforeInstructionWith(MInstruction *ins,
                                   vector<MInstruction *> instrs);

  void insertAfterInstructionWith(MInstruction *ins,
                                  vector<MInstruction *> instrs);

  vector<MInstruction*> getAllInstructions();
  vector<unique_ptr<MInstruction>> &getInstructions();
  vector<unique_ptr<MInstruction>> &getJmps();

  friend std::ostream &operator<<(std::ostream &os, const MBasicBlock &obj);
};

class MGlobal {
private:
  GlobalVariable *global;

public:
  MGlobal(GlobalVariable *global) : global(global) {}
  string getName() const;

  friend std::ostream &operator<<(std::ostream &os, const MGlobal &obj);
};

class MModule;

class MFunction {
private:
  FuncType *type;
  unique_ptr<vector<unique_ptr<ParaRegister>>> parameters;
  string name;
  unique_ptr<vector<unique_ptr<MBasicBlock>>> basicBlocks;
  MBasicBlock *entry;
  MBasicBlock *exit;
  MModule* mod;

public:
  unique_ptr<vector<MBasicBlock*>> domtPreOrder;
  int stack_offset;
  MFunction(FuncType *fType, string name);
  MBasicBlock *addBasicBlock(string name);

  void setEntry(MBasicBlock *entry);
  MBasicBlock *getEntry();

  void setExit(MBasicBlock *exit);
  MBasicBlock *getExit();

  void setMod(MModule *mod);
  MModule *getMod();

  ParaRegister *getPara(int idx);
  uint getParaSize() {return parameters->size();}
  FuncType *getType();

  string getName() const;
  vector<unique_ptr<MBasicBlock>> &getBasicBlocks();

  vector<Register*> &getVariables();
  friend std::ostream &operator<<(std::ostream &os, const MFunction &obj);
};

class MModule {
private:
  unique_ptr<vector<unique_ptr<MGlobal>>> globalVariables;
  unique_ptr<vector<unique_ptr<MFunction>>> functions;
  bool if_ssa = true;

public:
  MModule();

  bool is_ssa() {return if_ssa;}
  void ssa_out();
  MFunction *addFunction(FuncType *funcType, string name);
  MGlobal *addGlobalVariable(GlobalVariable *global);
  MGlobal *addGlobalFloat(FloatConstant *f);

  vector<unique_ptr<MGlobal>> &getGlobals();
  vector<unique_ptr<MFunction>> &getFunctions();

  friend std::ostream &operator<<(std::ostream &os, const MModule &obj);
};

// in bytes
static uint32_t cal_size(const Type *tp) {
  switch (tp->getTypeTag()) {
  case TT_POINTER:
    return 8;
  case TT_INT1:
  case TT_INT32:
  case TT_FLOAT:
    return 4;
  case TT_ARRAY: {
    const ArrayType *atp = static_cast<const ArrayType *>(tp);
    return atp->getLen() * cal_size(atp->getElemType());
  }
  case TT_FUNCTION:
  case TT_VOID:
  default:
    assert(0);
  }
  return 0;
}