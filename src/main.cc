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
  //       cond = -1;
  //   return x;
  // }
  // Type* i32Type = Type::getInt32Type();
  // ArrayType* arrType = Type::getArrayType(10, Type::getInt32Type());

  // FuncType* funcType = Type::getFuncType(i32Type);
  // Function* mainFunc = module->addFunction(funcType, "main");
  // BasicBlock* entryBB = module->addBasicBlock(mainFunc, "entry");
  // module->setCurrBasicBlock(entryBB);
  // AllocaInst* arrAlloc = module->addAllocaInst(arrType, "arr.addr");
  // AllocaInst* allocx = module->addAllocaInst(i32Type, "x.addr");
  // AllocaInst* alloccond = module->addAllocaInst(i32Type, "cond.addr");
  // module->addStoreInst(IntegerConstant::getConstInt(1), alloccond);

  // GetElemPtrInst* gepInst =
  //     module->addGetElemPtrInst(arrAlloc, IntegerConstant::getConstInt(0),
  //                               IntegerConstant::getConstInt(1), "arr1");
  // StoreInst* arrStore =
  //     module->addStoreInst(IntegerConstant::getConstInt(4), gepInst);

  // LoadInst* arrLoad = module->addLoadInst(gepInst, "arr1.val");
  // LoadInst* condVal = module->addLoadInst(alloccond, "cond.val");

  // BasicBlock* ifThenBB = module->addBasicBlock(mainFunc, "if.then");
  // BasicBlock* ifElseBB = module->addBasicBlock(mainFunc, "if.else");
  // BasicBlock* exit = module->addBasicBlock(mainFunc, "exit");
  // IcmpInst* icmp = module->addIcmpInst(
  //     SGT, condVal, IntegerConstant::getConstInt(0), "cmp.ret");
  // module->addBranchInst(icmp, ifThenBB, ifElseBB);

  // module->setCurrBasicBlock(ifThenBB);
  // module->addStoreInst(IntegerConstant::getConstInt(1), allocx);
  // module->addJumpInst(exit);

  // module->setCurrBasicBlock(ifElseBB);
  // module->addStoreInst(IntegerConstant::getConstInt(-1), alloccond);
  // module->addJumpInst(exit);

  // module->setCurrBasicBlock(exit);
  // LoadInst* ret = module->addLoadInst(allocx, "result");
  // module->addReturnInst(ret);

  /**
   * int foo(int x, int y, int z) {
   *   int r1 = x + y;
   *   if (x == 0) {
   *     int r2 = y + z;
   *   } else {
   *     int r3 = x + y; // shoule be remove
   *   }
   *   int r4 = y + x; // should be remove
   *   int r5 = y + z;
   *   return r4;
   * }
   */

  vector<Argument*> args;
  args.push_back(new Argument("x", Type::getInt32Type()));
  args.push_back(new Argument("y", Type::getInt32Type()));
  args.push_back(new Argument("z", Type::getInt32Type()));
  FuncType* funcType = Type::getFuncType(Type::getInt32Type(), args);

  Function* fFunction = module->addFunction(funcType, "foo");
  // foo.entry:
  BasicBlock* bb1 = module->addBasicBlock(fFunction, "foo.entry");
  module->setCurrBasicBlock(bb1);

  BinaryOpInst* r1 = module->addBinaryOpInst(ADD, funcType->getArgument(0),
                                             funcType->getArgument(1), "r1");

  IcmpInst* ficmp = module->addIcmpInst(
      EQ, funcType->getArgument(0), IntegerConstant::getConstInt(0), "cmp.ret");

  BasicBlock* trueBlock = module->addBasicBlock(fFunction, "if.then");
  BasicBlock* falseBlock = module->addBasicBlock(fFunction, "if.else");
  module->addBranchInst(ficmp, trueBlock, falseBlock);

  BasicBlock* fexit = module->addBasicBlock(fFunction, "exit");

  module->setCurrBasicBlock(trueBlock);
  module->addBinaryOpInst(ADD, funcType->getArgument(1),
                          funcType->getArgument(2), "r2");
  module->addJumpInst(fexit);

  module->setCurrBasicBlock(falseBlock);
  module->addBinaryOpInst(ADD, funcType->getArgument(0),
                          funcType->getArgument(1), "r3");
  module->addJumpInst(fexit);

  module->setCurrBasicBlock(fexit);
  BinaryOpInst* r4 = module->addBinaryOpInst(ADD, funcType->getArgument(1),
                                             funcType->getArgument(0), "r4");
  module->addBinaryOpInst(ADD, funcType->getArgument(1),
                          funcType->getArgument(2), "r5");

  module->addReturnInst(r4);

  module->irOptimize();

  Value* argx = funcType->getArgument(0);
  Use* use = argx->getUseHead();
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