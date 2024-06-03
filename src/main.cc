#include "main.hh"

#include <iostream>

#include "opt/example.hh"

int main() {

  // 以下代码仅为测试用，直接new对象容易内存泄漏，
  // 还需要在Module封装一层接口

  ANTPIE::Module* module = new ANTPIE::Module();

  /**
   * int foo(int x) {
   *   int arr[10];
   *   arr[1] = x;
   *   int ret = arr[1];
   *   if (5 <= ret) {
   *     ret = 5;
   *   }
   *   return ret;
   * }
  */
  FuncType* fFuncType = new FuncType(Type::getInt32Type());
  Argument* argument = new Argument("x", Type::getInt32Type());
  fFuncType->pushArgument(argument);
  Function* fFunction = new Function(fFuncType, "foo");
  BasicBlock* bb1 = new BasicBlock("foo.entry");
  fFunction->pushBasicBlock(bb1);
  module->setCurrBasicBlock(bb1);
  module->pushFunction(fFunction);

  ArrayType* arrType = new ArrayType(10, Type::getInt32Type());
  AllocaInst* arrAlloc = new AllocaInst(arrType, "arr.addr");
  module->pushInstrution(arrAlloc);

  GetElemPtrInst* gepInst = new GetElemPtrInst(arrAlloc, new IntegerConstant(0),
                                               new IntegerConstant(1), "arr1");
  module->pushInstrution(gepInst);

  StoreInst* arrStore = new StoreInst(argument, gepInst);
  module->pushInstrution(arrStore);

  LoadInst* arrLoad = new LoadInst(gepInst, "arr1.val");
  module->pushInstrution(arrLoad);

  BasicBlock* ifThenBB = new BasicBlock("if.then");
  BasicBlock* ifEndBB = new BasicBlock("if.end");
  fFunction->pushBasicBlock(ifThenBB);
  fFunction->pushBasicBlock(ifEndBB);

  IcmpInst* icmp =
      new IcmpInst(SLE, new IntegerConstant(5), arrLoad, "icmp.ret1");
  module->pushInstrution(icmp);

  BranchInst* br = new BranchInst(icmp, ifThenBB, ifEndBB);
  module->pushInstrution(br);

  // In if.then block, let ret1 = 5
  module->setCurrBasicBlock(ifThenBB);
  BinaryOpInst* ret1 = new BinaryOpInst(ADD, new IntegerConstant(5),
                                       new IntegerConstant(0), "ret1");
  module->pushInstrution(ret1);

  JumpInst* jump = new JumpInst(ifEndBB);
  module->pushInstrution(jump);

  // In if.end block, let ret = phi [arrLoad entry], [ret1 if.then]
  module->setCurrBasicBlock(ifEndBB);
  
  PhiInst* phi = new PhiInst("ret");
  phi->pushIncoming(arrLoad,bb1);
  phi->pushIncoming(ret1, ifThenBB);
  module->pushInstrution(phi);

  ReturnInst* rInst = new ReturnInst(phi);
  module->pushInstrution(rInst);

  // main function, call foo() at the end
  FuncType* mFuncType = new FuncType(Type::getInt32Type());
  Function* mFunction = new Function(mFuncType, "main");
  BasicBlock* basicBlock = new BasicBlock("entry");
  mFunction->pushBasicBlock(basicBlock);
  module->pushFunction(mFunction);
  module->setCurrBasicBlock(basicBlock);

  AllocaInst* allocaInst = new AllocaInst(Type::getInt32Type(), "a.addr");
  module->pushInstrution(allocaInst);

  IntegerConstant* intConstant = new IntegerConstant(10);

  StoreInst* storeInst = new StoreInst(intConstant, allocaInst);
  module->pushInstrution(storeInst);

  LoadInst* LoadInst = new class LoadInst(allocaInst, "a");
  module->pushInstrution(LoadInst);

  BinaryOpInst* addInstr =
      new BinaryOpInst(ADD, LoadInst, intConstant, "addInstr");
  module->pushInstrution(addInstr);

  BasicBlock* basicBlock2 = new BasicBlock("L1");
  mFunction->pushBasicBlock(basicBlock2);

  JumpInst* jumpInstr = new JumpInst(basicBlock2);
  module->pushInstrution(jumpInstr);

  module->setCurrBasicBlock(basicBlock2);

  IcmpInst* icmpInst = new IcmpInst(EQ, LoadInst, intConstant, "icmp.ret");
  module->pushInstrution(icmpInst);

  BasicBlock* basicBlock3 = new BasicBlock("L2");
  mFunction->pushBasicBlock(basicBlock3);

  BranchInst* brInstr = new BranchInst(icmpInst, basicBlock3, basicBlock2);
  module->pushInstrution(brInstr);

  module->setCurrBasicBlock(basicBlock3);

  SitofpInst* i2fInst = new SitofpInst(intConstant, "fp10");
  module->pushInstrution(i2fInst);

  FcmpInst* fcmpInst = new FcmpInst(OLE, i2fInst, i2fInst, "fcmp.ret");
  module->pushInstrution(fcmpInst);

  ZextInst* zextInst = new ZextInst(fcmpInst, Type::getInt32Type(), "ext.ret");
  module->pushInstrution(zextInst);

  CallInst* callInst = new CallInst(fFunction, "foo.ret");
  callInst->pushArgument(new IntegerConstant(4));
  module->pushInstrution(callInst);

  ReturnInst* returnInst = new ReturnInst(callInst);
  module->pushInstrution(returnInst);

  std::ofstream out_file;
  out_file.open("tests/test.ll");
  module->printIR(out_file);
}