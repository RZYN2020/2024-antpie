#include "Instruction.hh"
#include "Function.hh"
#include "BasicBlock.hh"
#include "Module.hh"
#include "Constant.hh"
#include "Machine.hh"
#include "select_instruction.hh"
#include "allocate_register.hh"
#include "prelude_conclusion.hh"

#include <fstream>
#include <iostream>

int main() {
  // ir使用示例，可以删除

  ANTPIE::Module* module = new ANTPIE::Module();

  /**
   * int gx = 5;
   * int garr[10] = {0, 0, 5, 3};
   * int empty[10];
   * float gf = 1.5;
   * int foo(int x) {
   *   int arr[10];
   *   arr[1] = x;
   *   int ret = arr[1];
   *   if (5 <= ret) {
   *     ret = 5;
   *   }
   *   ret = ret + gx;
   *   return ret;
   * }
   */

  // add global variable
  // @gx = dso_local global i32 0
  GlobalVariable* gv = module->addGlobalVariable(Type::getInt32Type(), "gx");
  // gloabl array
  ArrayType* arrType = Type::getArrayType(10, Type::getInt32Type());
  ArrayConstant* array = ArrayConstant::getConstArray(arrType);
  array->put(2, new IntegerConstant(5));
  array->put(3, new IntegerConstant(3));
  // @garr = dso_local global [10 x i32] [i32 0, i32 0, i32 5, i32 3, i32 0, i32
  // 0, i32 0, i32 0, i32 0, i32 0]
  GlobalVariable* garr = module->addGlobalVariable(arrType, array, "garr");
  // global empty array
  // @empty = dso_local global [10 x i32] zeroinitializer
  GlobalVariable* emptyArr = module->addGlobalVariable(arrType, "empty");
  // @gf = dso_local global float 1.500000
  GlobalVariable* gf = module->addGlobalVariable(
      Type::getFloatType(), FloatConstant::getConstFloat(1.5), "gf");

  vector<Argument*> args;
  Argument* argument = new Argument("x", Type::getInt32Type());
  args.push_back(argument);
  FuncType* funcType = Type::getFuncType(Type::getInt32Type(), args);
  // define dso_local i32 @foo(i32 %x) {
  Function* fFunction = module->addFunction(funcType, "foo");
  // foo.entry:
  BasicBlock* bb1 = module->addBasicBlock(fFunction, "foo.entry");
  module->setCurrBasicBlock(bb1);
  // %arr.addr = alloca [10 x i32]
  AllocaInst* arrAlloc = module->addAllocaInst(arrType, "arr.addr");
  // %arr1 = getelementptr inbounds [10 x i32], [10 x i32]* %arr.addr, i32 0, i32 1
  GetElemPtrInst* gepInst =
      module->addGetElemPtrInst(arrAlloc, IntegerConstant::getConstInt(0),
                                IntegerConstant::getConstInt(1), "arr1");
  // store i32 %x, i32* %arr1
  StoreInst* arrStore = module->addStoreInst(argument, gepInst);
  // %arr1.val = load i32, i32* %arr1
  LoadInst* arrLoad = module->addLoadInst(gepInst, "arr1.val");
  // %icmp.ret1 = icmp sle i32 5, %arr1.val
  IcmpInst* icmp = module->addIcmpInst(SLE, IntegerConstant::getConstInt(5),
                                       arrLoad, "icmp.ret1");

  BasicBlock* ifThenBB = module->addBasicBlock(fFunction, "if.then");
  BasicBlock* ifEndBB = module->addBasicBlock(fFunction, "if.end");
  // br i1 %icmp.ret1, label %if.then, label %if.end
  BranchInst* br = module->addBranchInst(icmp, ifThenBB, ifEndBB);

  // In if.then block, let ret1 = 5
  module->setCurrBasicBlock(ifThenBB);
  // %ret1 = add i32 5, 0
  BinaryOpInst* ret1 =
      module->addBinaryOpInst(ADD, IntegerConstant::getConstInt(5),
                              IntegerConstant::getConstInt(0), "ret1");
  // br label %if.end
  JumpInst* jump = module->addJumpInst(ifEndBB);

  // In if.end block, let ret = phi [arrLoad entry], [ret1 if.then]
  module->setCurrBasicBlock(ifEndBB);
  // %ret = phi i32[ %arr1.val, %foo.entry ], [ %ret1, %if.then ]
  PhiInst* phi = module->addPhiInst("ret");
  phi->pushIncoming(arrLoad, bb1);
  phi->pushIncoming(ret1, ifThenBB);
  // %gv.val = load i32, i32* @gx
  LoadInst* gxLoad = module->addLoadInst(gv, "gv.val");
  // %ret2 = add i32 %ret, %gv.val
  BinaryOpInst* retAddInstr = module->addBinaryOpInst(ADD, phi, gxLoad, "ret2");
  // ret i32 %ret2
  ReturnInst* rInst = module->addReturnInst(retAddInstr);

  // main function, call foo() at the end
  FuncType* mFuncType = Type::getFuncType(Type::getInt32Type());
  // define dso_local i32 @main() {
  Function* mFunction = module->addFunction(mFuncType, "main");
  BasicBlock* basicBlock = module->addBasicBlock(mFunction, "entry");
  module->setCurrBasicBlock(basicBlock);
  // %a.addr = alloca i32
  AllocaInst* allocaInst =
      module->addAllocaInst(Type::getInt32Type(), "a.addr");

  IntegerConstant* intConstant = IntegerConstant::getConstInt(10);
  // store i32 10, i32* %a.addr
  StoreInst* storeInst = module->addStoreInst(intConstant, allocaInst);
  // %a = load i32, i32* %a.addr
  LoadInst* LoadInst = module->addLoadInst(allocaInst, "a");
  // %addInstr = add i32 %a, 10
  BinaryOpInst* addInstr =
      module->addBinaryOpInst(ADD, LoadInst, intConstant, "addInstr");

  BasicBlock* basicBlock2 = module->addBasicBlock(mFunction, "L1");
  // br label %L1
  JumpInst* jumpInstr = module->addJumpInst(basicBlock2);
  // L1:
  module->setCurrBasicBlock(basicBlock2);
  // %icmp.ret = icmp eq i32 %a, 10
  IcmpInst* icmpInst =
      module->addIcmpInst(EQ, LoadInst, intConstant, "icmp.ret");

  BasicBlock* basicBlock3 = module->addBasicBlock(mFunction, "L2");
  // br i1 %icmp.ret, label %L2, label %L1
  BranchInst* brInstr =
      module->addBranchInst(icmpInst, basicBlock3, basicBlock2);
  // L2:
  module->setCurrBasicBlock(basicBlock3);
  // %fp10 = sitofp i32 10 to float
  SitofpInst* i2fInst = module->addSitofpInst(intConstant, "fp10");
  // %fcmp.ret = fcmp ole float %fp10, %fp10
  FcmpInst* fcmpInst = module->addFcmpInst(OLE, i2fInst, i2fInst, "fcmp.ret");
  // %ext.ret = zext i1 %fcmp.ret to i32
  ZextInst* zextInst =
      module->addZextInst(fcmpInst, Type::getInt32Type(), "ext.ret");

  vector<Value*> callArgs;
  callArgs.push_back(IntegerConstant::getConstInt(4));
  // %foo.ret = call i32 @foo(i32 4)
  CallInst* callInst = module->addCallInst(fFunction, callArgs, "foo.ret");
  // ret i32 %foo.ret
  ReturnInst* returnInst = module->addReturnInst(callInst);


  MachineModule* mmodule = new MachineModule();
  
  select_instruction(mmodule, module);
  mmodule->printASM(std::cout);

  allocate_register(mmodule);
  mmodule->printASM(std::cout);

//   prelude_conclusion(mmodule);
//   mmodule->printASM(std::cout);

  std::ofstream out_file;
  out_file.open("tests/test.ll");
  module->printIR(out_file);
}