#include <fstream>
#include <iostream>

#include "BasicBlock.hh"
#include "Constant.hh"
#include "DomTree.hh"
#include "Function.hh"
#include "Instruction.hh"
#include "Machine.hh"
#include "Module.hh"
#include "allocate_register.hh"
#include "prelude_conclusion.hh"
#include "select_instruction.hh"

int main() {
  ANTPIE::Module* module = new ANTPIE::Module();
  // src code:
  //
  // int main() {
  //   int x, cond = 1;
  //   if (cond > 0)
  //       x = 1;
  //   else
  //       x = -1;
  //   return x;
  // }
  Type* i32Type = Type::getInt32Type();
  ArrayType* arrType = Type::getArrayType(10, Type::getInt32Type());

  FuncType* funcType = Type::getFuncType(i32Type);
  Function* mainFunc = module->addFunction(funcType, "main");
  BasicBlock* entryBB = module->addBasicBlock(mainFunc, "entry");
  module->setCurrBasicBlock(entryBB);
  AllocaInst* arrAlloc = module->addAllocaInst(arrType, "arr.addr");
  AllocaInst* allocx = module->addAllocaInst(i32Type, "x.addr");
  AllocaInst* alloccond = module->addAllocaInst(i32Type, "cond.addr");
  module->addStoreInst(IntegerConstant::getConstInt(1), alloccond);
  
  GetElemPtrInst* gepInst =
      module->addGetElemPtrInst(arrAlloc, IntegerConstant::getConstInt(0),
                                IntegerConstant::getConstInt(1), "arr1");
  StoreInst* arrStore =
      module->addStoreInst(IntegerConstant::getConstInt(4), gepInst);

  LoadInst* arrLoad = module->addLoadInst(gepInst, "arr1.val");
  LoadInst* condVal = module->addLoadInst(alloccond, "cond.val");

  BasicBlock* ifThenBB = module->addBasicBlock(mainFunc, "if.then");
  BasicBlock* ifElseBB = module->addBasicBlock(mainFunc, "if.else");
  BasicBlock* exit = module->addBasicBlock(mainFunc, "exit");
  IcmpInst* icmp = module->addIcmpInst(
      SGT, condVal, IntegerConstant::getConstInt(0), "cmp.ret");
  module->addBranchInst(icmp, ifThenBB, ifElseBB);

  module->setCurrBasicBlock(ifThenBB);
  module->addStoreInst(IntegerConstant::getConstInt(1), allocx);
  module->addJumpInst(exit);

  module->setCurrBasicBlock(ifElseBB);
  module->addStoreInst(IntegerConstant::getConstInt(-1), allocx);
  module->addJumpInst(exit);

  module->setCurrBasicBlock(exit);
  LoadInst* ret = module->addLoadInst(allocx, "result");
  module->addReturnInst(ret);

  module->irOptimize();
  // module->buildCFG();

  //   MachineModule* mmodule = new MachineModule();

  //   select_instruction(mmodule, module);
  //   mmodule->printASM(std::cout);

  //   allocate_register(mmodule);
  //   mmodule->printASM(std::cout);

  //   prelude_conclusion(mmodule);
  //   mmodule->printASM(std::cout);

  std::ofstream out_file;
  out_file.open("tests/test.ll");
  module->printIR(out_file);
}