//
// Created by yanayn on 7/10/24.
//

#ifndef ANTPIE_MYSYSYPARSERVISITOR_H
#define ANTPIE_MYSYSYPARSERVISITOR_H

#include <functional>

#include "AggregateValue.hh"
#include "Constant.hh"
#include "Module.hh"
#include "SysYParserBaseVisitor.h"
#include "Type.hh"
#include "VariableTable.h"
#include "antlr4-runtime.h"

#define any antlrcpp::Any

typedef struct whileBlockInfo {
 public:
  BasicBlock* condBlock;
  BasicBlock* exitBlock;
  whileBlockInfo(BasicBlock* cond, BasicBlock* exit) {
    this->condBlock = cond;
    this->exitBlock = exit;
  };
} WhileBlockInfo;

using ANTPIE::Module;
using std::stack;
using std::stof;
using std::stoi;
using std::string;
using std::vector;

class MySysYParserVisitor : SysYParserBaseVisitor {
 public:
  MySysYParserVisitor(VariableTable* current);
  void setArrayInitVal(Value* arr, ArrayConstant* arrayValue);

  void setModule(Module m);
  Module module;
  VariableTable* current;
  Type* currDefType;  // 当前的定义的类型，在多重定义的时候使用
  Value* currentRet;
  Type* currentRetType;
  Type* functionRetType;
  BasicBlock* trueBasicBlock;
  BasicBlock* falseBasicBlock;
  stack<WhileBlockInfo*> loopLayer;
  Function* currentIn;
  AggregateValue* currAggregate;
  map<string, OpTag> Integer_Operator_OpTag_Table;
  map<string, OpTag> F_Operator_OpTag_Table;
  map<string, OpTag> Logic_Operator_OpTag_Table;

  antlrcpp::Any visitProgram(SysYParserParser::ProgramContext* ctx) override {
    Integer_Operator_OpTag_Table = map<string, OpTag>();
    Logic_Operator_OpTag_Table = map<string, OpTag>();
    Integer_Operator_OpTag_Table["+"] = OpTag::ADD;
    Integer_Operator_OpTag_Table["-"] = OpTag::SUB;
    Integer_Operator_OpTag_Table["*"] = OpTag::MUL;
    Integer_Operator_OpTag_Table["/"] = OpTag::SDIV;
    Integer_Operator_OpTag_Table["%"] = OpTag::SREM;
    Integer_Operator_OpTag_Table[">"] = OpTag::SGT;
    Integer_Operator_OpTag_Table["<"] = OpTag::SLT;
    Integer_Operator_OpTag_Table[">="] = OpTag::SGE;
    Integer_Operator_OpTag_Table["<="] = OpTag::SLE;
    Logic_Operator_OpTag_Table["!="] = OpTag::NE;
    Logic_Operator_OpTag_Table["=="] = OpTag::EQ;
    Logic_Operator_OpTag_Table["&&"] = OpTag::AND;
    Logic_Operator_OpTag_Table["||"] = OpTag::OR;
    F_Operator_OpTag_Table = map<string, OpTag>();
    F_Operator_OpTag_Table["+"] = OpTag::FADD;
    F_Operator_OpTag_Table["-"] = OpTag::FSUB;
    F_Operator_OpTag_Table["*"] = OpTag::FMUL;
    F_Operator_OpTag_Table["/"] = OpTag::FDIV;
    F_Operator_OpTag_Table["%"] = OpTag::FREM;
    F_Operator_OpTag_Table[">"] = OpTag::OGT;
    F_Operator_OpTag_Table["<"] = OpTag::OLT;
    F_Operator_OpTag_Table[">="] = OpTag::OGE;
    F_Operator_OpTag_Table["<="] = OpTag::OLE;
    loopLayer = stack<WhileBlockInfo*>();
    current = new VariableTable(nullptr);
    visit(ctx->compUnit());
    functionRetType = nullptr;
    return nullptr;
  }
  void buildCondition() {
    Int1Type* intType = dynamic_cast<Int1Type*>(currentRet->getType());
    if (intType != nullptr) {
      currentRet = module.addIcmpInst(OpTag::NE, currentRet,
                                      new IntegerConstant(0), "icmp eq");
    }
    module.addBranchInst(currentRet, trueBasicBlock, falseBasicBlock);
  }

  antlrcpp::Any visitInitLVal(SysYParserParser::InitLValContext* ctx) override {
    int dimesion = ctx->constExp().size();
    Type* t = currDefType;
    for (int i = dimesion - 1; i >= 0; i--) {
      visit(ctx->constExp(i));
      t = new ArrayType(stoi(currentRet->toString()), t);
    }
    currDefType = t;
    return nullptr;
  }
  antlrcpp::Any visitConstDefSingle(
      SysYParserParser::ConstDefSingleContext* ctx) override {
    string varName = ctx->Identifier()->getText();
    visit(ctx->constInitVal());
    if (currDefType->getTypeTag() == TT_FLOAT) {
      float f = stof(currentRet->toString());
      current->put(varName, new FloatConstant(f));
    } else {
      int i = stoi(currentRet->toString());
      current->put(varName, new IntegerConstant(i));
    }
    return nullptr;
  }
  antlrcpp::Any visitConstDefArray(
      SysYParserParser::ConstDefArrayContext* ctx) override {
    visit(ctx->initLVal());
    // currentRetType=Array.type
    currAggregate = new AggregateValue((ArrayType*)currDefType);
    visit(ctx->constInitVal());
    String varName = ctx->initLVal()->Identifier()->getText();
    buildVariable(varName, currDefType);
    setArrayInitVal(current->get(varName), currAggregate->toArrayConstant());
    return nullptr;
  }

  antlrcpp::Any visitInitValArray(
      SysYParserParser::InitValArrayContext* ctx) override {
    for (auto* elem : ctx->initVal()) {
      if (auto* initVarSingle =
              dynamic_cast<SysYParserParser::InitValSingleContext*>(elem)) {
        visit(initVarSingle->exp());
        currAggregate->pushValue(currentRet);
      } else {
        currAggregate->inBrace();
        visit(elem);
      }
    }
    currAggregate->outBrace();
    return nullptr;
  }

  antlrcpp::Any visitConstInitValArray(
      SysYParserParser::ConstInitValArrayContext* ctx) override {
    for (auto* elem : ctx->constInitVal()) {
      if (auto* initVarSingle =
              dynamic_cast<SysYParserParser::ConstInitValSingleContext*>(
                  elem)) {
        visit(initVarSingle->constExp());
        currAggregate->pushValue(currentRet);
      } else {
        currAggregate->inBrace();
        visit(elem);
      }
    }
    currAggregate->outBrace();
    return nullptr;
  }

  antlrcpp::Any visitVarDefSingle(
      SysYParserParser::VarDefSingleContext* ctx) override {
    string varName = ctx->Identifier()->getText();
    buildVariable(varName, currDefType);
    return nullptr;
  }
  antlrcpp::Any visitVarDefArray(
      SysYParserParser::VarDefArrayContext* ctx) override {
    antlr4::tree::AbstractParseTreeVisitor::visit(ctx->initLVal());
    buildVariable(ctx->initLVal()->Identifier()->getText(), currDefType);
    return nullptr;
  }
  antlrcpp::Any visitVarDefSingleInitVal(
      SysYParserParser::VarDefSingleInitValContext* ctx) override {
    string varName = ctx->Identifier()->getText();
    visit(ctx->initVal());
    buildVariable(varName, currentRet);

    return nullptr;
  }
  antlrcpp::Any visitVarDefArrayInitVal(
      SysYParserParser::VarDefArrayInitValContext* ctx) override {
    visit(ctx->initLVal());
    // currentRetType=Array.type
    currAggregate = new AggregateValue((ArrayType*)currDefType);
    visit(ctx->initVal());
    String varName = ctx->initLVal()->Identifier()->getText();
    buildVariable(varName, currDefType);
    setArrayInitVal(current->get(varName), currAggregate->toArrayConstant());
    return nullptr;
  }
  antlrcpp::Any visitConstDecl(
      SysYParserParser::ConstDeclContext* ctx) override {
    if ((ctx->bType()->getText().compare("int")) == 0) {
      currDefType = new Int32Type();
    } else {
      currDefType = new FloatType();
    }
    for (SysYParserParser::ConstDefContext* c : ctx->constDef()) {
      visit(c);
    }
    return nullptr;
  }
  antlrcpp::Any visitVarDecl(SysYParserParser::VarDeclContext* ctx) override {
    if ((ctx->bType()->getText().compare("int")) == 0) {
      currDefType = new Int32Type();
    } else {
      currDefType = new FloatType();
    }
    for (SysYParserParser::VarDefContext* c : ctx->varDef()) {
      visit(c);
    }
    return nullptr;
  }
  // TODO: SysYParserParser::visitConstInitValContext()等同类的模块
  antlrcpp::Any visitFuncDef(SysYParserParser::FuncDefContext* ctx) override {
    if (ctx->funcType()->getText().compare("int") == 0) {
      functionRetType = new Int32Type();
    } else if (ctx->funcType()->getText().compare("float") == 0) {
      functionRetType = new FloatType();
    } else {
      functionRetType = new VoidType();
    }
    current = new VariableTable(current);
    vector<Argument*> args = vector<Argument*>();
    if (ctx->funcFParams() != nullptr) {
      // 在函数参数定义范围内
      for (SysYParserParser::FuncFParamContext* co :
           ctx->funcFParams()->funcFParam()) {
        SysYParserParser::FuncFParamSingleContext* st =
            dynamic_cast<SysYParserParser::FuncFParamSingleContext*>(co);
        if (st == nullptr) {
          SysYParserParser::FuncFParamArrayContext* ac =
              dynamic_cast<SysYParserParser::FuncFParamArrayContext*>(co);
          Type* bt;
          if (ac->bType()->getText().compare("int") == 0) {
            bt = Type::getInt32Type();
          } else {
            bt = Type::getFloatType();
          }
          int dimens = ac->exp().size();
          for (int i = dimens - 1; i >= 0; i--) {
            visit(ac->exp(i));
            bt = new ArrayType(stoi(currentRet->toString()), bt);
          }
          Type* prt = new PointerType(bt);
          Argument* argu = new Argument(ac->Identifier()->getText(), prt);
          args.push_back(argu);
          current->put(ac->Identifier()->getText(), argu);
        } else {
          string nm = st->Identifier()->getText();
          Argument* argu;
          if (st->bType()->getText().compare("int") == 0) {
            argu = new Argument(nm, Type::getInt32Type());
          } else {
            argu = new Argument(nm, Type::getFloatType());
          }
          args.push_back(argu);
        }
      }
    }
    Type* rt;
    if (ctx->funcType()->getText() == "int") {
      rt = Type::getInt32Type();
    } else if (ctx->funcType()->getText() == "float") {
      rt = Type::getFloatType();
    } else {
      rt = Type::getVoidType();
    }
    FuncType* funcType = Type::getFuncType(rt, args);
    String funcName = ctx->Identifier()->getText();
    Function* mFunction = module.addFunction(funcType, funcName);
    current->parent->put(funcName, mFunction);
    currentIn = mFunction;
    BasicBlock* basicBlock = module.addBasicBlock(mFunction, "entry");
    module.setCurrBasicBlock(basicBlock);

    // set argument alloca
    for (Argument* arg : args) {
      if (arg->getType()->getTypeTag() != TT_POINTER) {
        buildVariable(arg->getName(), arg);
      }
    }
    for (SysYParserParser::BlockItemContext* bc : ctx->block()->blockItem()) {
      visit(bc);
    }
    current = current->parent;
    // 退出函数参数定义范围内
    return nullptr;
  }

  antlrcpp::Any visitBlock(SysYParserParser::BlockContext* ctx) override {
    VariableTable* scope = new VariableTable(current);
    current = scope;
    for (SysYParserParser::BlockItemContext* c : ctx->blockItem()) {
      visit(c);
    }
    current = current->parent;
    return nullptr;
  }

  antlrcpp::Any visitStmtCond(SysYParserParser::StmtCondContext* ctx) override {
    BasicBlock* trueBlock = module.addBasicBlock(currentIn, "if.true");
    BasicBlock* falseBlock = module.addBasicBlock(currentIn, "if.false");
    BasicBlock* exitBlock;
    if (ctx->stmt().size() == 2) {
      exitBlock = module.addBasicBlock(currentIn, "if.end");
    } else {
      exitBlock = falseBlock;
    }
    trueBasicBlock = trueBlock;
    falseBasicBlock = falseBlock;
    visit(ctx->cond());
    module.setCurrBasicBlock(trueBlock);
    visit(ctx->stmt(0));
    module.addJumpInst(exitBlock);
    if (ctx->stmt(1) != nullptr) {
      visit(ctx->stmt(1));
      module.addJumpInst(exitBlock);
      module.setCurrBasicBlock(exitBlock);
    }
    return nullptr;
  }

  antlrcpp::Any visitStmtWhile(
      SysYParserParser::StmtWhileContext* ctx) override {
    BasicBlock* condBlock = module.addBasicBlock(currentIn, "while.begin");
    BasicBlock* exitBlock = module.addBasicBlock(currentIn, "while.end");
    BasicBlock* bodyBlock = module.addBasicBlock(currentIn, "while.loop");
    module.addJumpInst(condBlock);
    module.setCurrBasicBlock(condBlock);
    trueBasicBlock = bodyBlock;
    falseBasicBlock = exitBlock;
    visit(ctx->cond());
    loopLayer.push(new whileBlockInfo(condBlock, exitBlock));
    module.setCurrBasicBlock(bodyBlock);
    visit(ctx->stmt());
    module.addJumpInst(condBlock);
    module.setCurrBasicBlock(exitBlock);
    loopLayer.pop();
    return nullptr;
  }

  antlrcpp::Any visitStmtBreak(
      SysYParserParser::StmtBreakContext* ctx) override {
    module.addJumpInst(loopLayer.top()->exitBlock);
    module.addBasicBlock(currentIn, "after_while_break");
    return nullptr;
  }

  antlrcpp::Any visitStmtContinue(
      SysYParserParser::StmtContinueContext* ctx) override {
    module.addJumpInst(loopLayer.top()->condBlock);
    module.addBasicBlock(currentIn, "after_while_continue");
    return nullptr;
  }
  antlrcpp::Any visitStmtReturn(
      SysYParserParser::StmtReturnContext* ctx) override {
    if (ctx->exp() != nullptr) {
      visit(ctx->exp());
      module.addReturnInst(currentRet);
    } else {
      module.addReturnInst();
    }
    return nullptr;
  }
  antlrcpp::Any visitLOrExp(SysYParserParser::LOrExpContext* ctx) override {
    BasicBlock* falseBlock = falseBasicBlock;
    for (int i = 0; i < ctx->lAndExp().size(); i++) {
      if (i == ctx->lAndExp().size() - 1) {
        falseBasicBlock = falseBlock;
        visit(ctx->lAndExp(i));
      } else {
        BasicBlock* nextBlock = module.addBasicBlock(currentIn, "OR COND");
        falseBasicBlock = nextBlock;
        visit(ctx->lAndExp(i));
        module.setCurrBasicBlock(nextBlock);
      }
    }
    return nullptr;
  }
  antlrcpp::Any visitLAndExp(SysYParserParser::LAndExpContext* ctx) override {
    BasicBlock* trueBlock = trueBasicBlock;
    for (int i = 0; i < ctx->eqExp().size(); i++) {
      if (i == ctx->eqExp().size() - 1) {
        trueBasicBlock = trueBlock;
        visit(ctx->eqExp(i));
        buildCondition();
      } else {
        BasicBlock* nextBlock = module.addBasicBlock(currentIn, "AND COND");
        trueBasicBlock = nextBlock;
        visit(ctx->eqExp(i));
        buildCondition();
        module.setCurrBasicBlock(nextBlock);
      }
    }
    return nullptr;
  }

  antlrcpp::Any visitUnaryExpFuncR(
      SysYParserParser::UnaryExpFuncRContext* ctx) override {
    vector<Value*> args = vector<Value*>();
    if (ctx->funcRParams()) {
      for (SysYParserParser::FuncRParamContext* c :
           ctx->funcRParams()->funcRParam()) {
        visit(c);
        args.push_back(currentRet);
      }
    }
    currentRet = module.addCallInst(
        (Function*)current->get(ctx->Identifier()->getText()), args,
        "call" + ctx->Identifier()->getText());
    return nullptr;
  }

  antlrcpp::Any visitAddExp(SysYParserParser::AddExpContext* ctx) override {
    visit(ctx->left);
    Value* l_Val = currentRet;
    Value* r_Val;
    if (l_Val && l_Val->isPointer()) {
      LoadInst* li = module.addLoadInst(l_Val, "lval");
      l_Val = li;
    }
    for (int i = 0; i < ctx->op.size(); ++i) {
      visit(ctx->right[i]);
      r_Val = currentRet;
      if (r_Val->isPointer()) {
        r_Val = module.addLoadInst(r_Val, "rval");
      }

      // lhs and rhs both are constant
      if (dynamic_cast<Constant*>(l_Val) && dynamic_cast<Constant*>(r_Val)) {
        if (l_Val->isa(VT_FLOATCONST) || r_Val->isa(VT_FLOATCONST)) {
          // result is float

          float lhs, rhs;
          if (l_Val->isa(VT_INTCONST)) {
            lhs = ((IntegerConstant*)l_Val)->getValue();
          } else {
            lhs = ((FloatConstant*)l_Val)->getValue();
          }
          if (r_Val->isa(VT_INTCONST)) {
            rhs = ((IntegerConstant*)r_Val)->getValue();
          } else {
            rhs = ((FloatConstant*)r_Val)->getValue();
          }
          float ret_float = 0;
          if (ctx->op[i]->getText() == "+") {
            ret_float = lhs + rhs;
          } else {
            ret_float = lhs - rhs;
          }
          currentRet = FloatConstant::getConstFloat(ret_float);
          return nullptr;
        } else {
          // result is integer
          int lhs = ((IntegerConstant*)l_Val)->getValue();
          int rhs = ((IntegerConstant*)r_Val)->getValue();
          int ret_int;
          if (ctx->op[i]->getText() == "+") {
            ret_int = lhs + rhs;
          } else {
            ret_int = lhs - rhs;
          }
          currentRet = IntegerConstant::getConstInt(ret_int);
          return nullptr;
        }
      }
      OpTag t;
      if ((l_Val->getType()->getTypeTag() == TypeTag::TT_FLOAT) ||
          (r_Val->getType()->getTypeTag() == TypeTag::TT_FLOAT)) {
        t = F_Operator_OpTag_Table[ctx->op[i]->getText()];
        if (l_Val->getType()->getTypeTag() == TypeTag::TT_INT32) {
          l_Val = module.addSitofpInst(l_Val, "lval");
        }
        if (r_Val->getType()->getTypeTag() == TypeTag::TT_INT32) {
          r_Val = module.addSitofpInst(r_Val, "lval");
        }
        l_Val = module.addBinaryOpInst(t, l_Val, r_Val, "addres");
      } else {
        t = Integer_Operator_OpTag_Table[ctx->op[i]->getText()];
        l_Val = module.addBinaryOpInst(t, l_Val, r_Val, "addres");
      }
    }
    currentRet = l_Val;
    return nullptr;
  }

  antlrcpp::Any visitMulExp(SysYParserParser::MulExpContext* ctx) override {
    visit(ctx->left);
    Value* l_Val = currentRet;
    Value* r_Val;
    if (l_Val && l_Val->isPointer()) {
      LoadInst* li = module.addLoadInst(l_Val, "lval");
      l_Val = li;
    }
    for (int i = 0; i < ctx->op.size(); ++i) {
      visit(ctx->right[i]);
      r_Val = currentRet;
      if (r_Val->isPointer()) {
        r_Val = module.addLoadInst(r_Val, "rval");
      }
      // lhs and rhs both are constant
      if (dynamic_cast<Constant*>(l_Val) && dynamic_cast<Constant*>(r_Val)) {
        if (l_Val->isa(VT_FLOATCONST) || r_Val->isa(VT_FLOATCONST)) {
          // result is float

          float lhs, rhs;
          if (l_Val->isa(VT_INTCONST)) {
            lhs = ((IntegerConstant*)l_Val)->getValue();
          } else {
            lhs = ((FloatConstant*)l_Val)->getValue();
          }
          if (r_Val->isa(VT_INTCONST)) {
            rhs = ((IntegerConstant*)r_Val)->getValue();
          } else {
            rhs = ((FloatConstant*)r_Val)->getValue();
          }
          float ret_float = 0;
          if (ctx->op[i]->getText() == "*") {
            ret_float = lhs * rhs;
          } else {
            ret_float = lhs / rhs;
          }
          currentRet = FloatConstant::getConstFloat(ret_float);
          return nullptr;
        } else {
          // result is integer
          int lhs = ((IntegerConstant*)l_Val)->getValue();
          int rhs = ((IntegerConstant*)r_Val)->getValue();
          int ret_int;
          if (ctx->op[i]->getText() == "*") {
            ret_int = lhs * rhs;
          } else {
            ret_int = lhs / rhs;
          }
          currentRet = IntegerConstant::getConstInt(ret_int);
          return nullptr;
        }
      }
      OpTag t;
      if ((l_Val->getType()->getTypeTag() == TypeTag::TT_FLOAT) ||
          (r_Val->getType()->getTypeTag() == TypeTag::TT_FLOAT)) {
        t = F_Operator_OpTag_Table[ctx->op[i]->getText()];
        if (l_Val->getType()->getTypeTag() == TypeTag::TT_INT32) {
          l_Val = module.addSitofpInst(l_Val, "lval");
        }
        if (r_Val->getType()->getTypeTag() == TypeTag::TT_INT32) {
          r_Val = module.addSitofpInst(r_Val, "lval");
        }
        l_Val = module.addBinaryOpInst(t, l_Val, r_Val, "addres");
      } else {
        t = Integer_Operator_OpTag_Table[ctx->op[i]->getText()];
        l_Val = module.addBinaryOpInst(t, l_Val, r_Val, "addres");
      }
    }
    currentRet = l_Val;
    return nullptr;
  }

  antlrcpp::Any visitRelExp(SysYParserParser::RelExpContext* ctx) override {
    visit(ctx->left);
    Value* l_Val = currentRet;
    Value* r_Val;
    if (l_Val && l_Val->isPointer()) {
      LoadInst* li = module.addLoadInst(l_Val, "lval");
      l_Val = li;
    }
    for (int i = 0; i < ctx->op.size(); ++i) {
      visit(ctx->right[i]);
      r_Val = currentRet;
      if (r_Val->isPointer()) {
        r_Val = module.addLoadInst(r_Val, "rval");
      }
      OpTag t;
      if ((l_Val->getType()->getTypeTag() == TypeTag::TT_FLOAT) ||
          (r_Val->getType()->getTypeTag() == TypeTag::TT_FLOAT)) {
        t = F_Operator_OpTag_Table[ctx->op[i]->getText()];
        if (l_Val->getType()->getTypeTag() == TypeTag::TT_INT32) {
          l_Val = module.addSitofpInst(l_Val, "lval");
        } else if (l_Val->getType()->getTypeTag() == TypeTag::TT_INT1) {
          l_Val = module.addZextInst(l_Val, Type::getInt32Type(), "lVal");
          l_Val = module.addSitofpInst(l_Val, "lval");
        }
        if (r_Val->getType()->getTypeTag() == TypeTag::TT_INT32) {
          r_Val = module.addSitofpInst(r_Val, "rval");
        } else if (r_Val->getType()->getTypeTag() == TypeTag::TT_INT1) {
          r_Val = module.addZextInst(r_Val, Type::getInt32Type(), "rVal");
          r_Val = module.addSitofpInst(r_Val, "rval");
        }
        if (i > 0) {
          l_Val = module.addZextInst(l_Val, Type::getInt32Type(), "lval");
        }
        l_Val = module.addBinaryOpInst(t, l_Val, r_Val, "addres");
      } else {
        t = Integer_Operator_OpTag_Table[ctx->op[i]->getText()];
        if (i > 0) {
          l_Val = module.addZextInst(l_Val, Type::getInt32Type(), "lval");
        }
        l_Val = module.addBinaryOpInst(t, l_Val, r_Val, "addres");
      }
    }
    currentRet = l_Val;
    return nullptr;
  }

  antlrcpp::Any visitEqExp(SysYParserParser::EqExpContext* ctx) override {
    visit(ctx->left);
    Value* l_Val = currentRet;
    Value* r_Val;
    if (l_Val && l_Val->isPointer()) {
      LoadInst* li = module.addLoadInst(l_Val, "lval");
      l_Val = li;
    }
    for (int i = 0; i < ctx->op.size(); ++i) {
      visit(ctx->right[i]);
      r_Val = currentRet;
      if (r_Val->isPointer()) {
        r_Val = module.addLoadInst(r_Val, "rval");
      }
      OpTag t = Logic_Operator_OpTag_Table[ctx->op[i]->getText()];
      if ((l_Val->getType()->getTypeTag() == TypeTag::TT_FLOAT) ||
          (r_Val->getType()->getTypeTag() == TypeTag::TT_FLOAT)) {
        if (l_Val->getType()->getTypeTag() == TypeTag::TT_INT32) {
          l_Val = module.addSitofpInst(l_Val, "lval");
        } else if (l_Val->getType()->getTypeTag() == TypeTag::TT_INT1) {
          l_Val = module.addZextInst(l_Val, Type::getInt32Type(), "lVal");
          l_Val = module.addSitofpInst(l_Val, "lval");
        }
        if (r_Val->getType()->getTypeTag() == TypeTag::TT_INT32) {
          r_Val = module.addSitofpInst(r_Val, "rval");
        } else if (r_Val->getType()->getTypeTag() == TypeTag::TT_INT1) {
          r_Val = module.addZextInst(r_Val, Type::getInt32Type(), "rVal");
          r_Val = module.addSitofpInst(r_Val, "rval");
        }
        if (i > 0) {
          l_Val = module.addZextInst(l_Val, Type::getInt32Type(), "lval");
        }
        l_Val = module.addBinaryOpInst(t, l_Val, r_Val, "addres");
      } else {
        if (i > 0) {
          l_Val = module.addZextInst(l_Val, Type::getInt32Type(), "lval");
        }
        l_Val = module.addBinaryOpInst(t, l_Val, r_Val, "addres");
      }
    }
    currentRet = l_Val;
    return nullptr;
  }
  antlrcpp::Any visitLValSingle(
      SysYParserParser::LValSingleContext* ctx) override {
    Value* get = current->get(ctx->getText());
    if (get->isPointer()) {
      currentRet = module.addLoadInst(get, "get");
    } else {
      currentRet = get;
    }
    return nullptr;
  }

  antlrcpp::Any visitLValArray(
      SysYParserParser::LValArrayContext* ctx) override {
    Value* ptr = current->get(ctx->Identifier()->getText());
    if (ptr->getValueTag() == ValueTag::VT_ARG) {
      visit(ctx->exp(0));
      ptr = module.addGetElemPtrInst(ptr, currentRet, "ret");
      for (int i = 1; i < ctx->exp().size(); ++i) {
        visit(ctx->exp(i));
        ptr = module.addGetElemPtrInst(ptr, new IntegerConstant(0), currentRet,
                                       "ret");
      }
    } else {
      for (int i = 0; i < ctx->exp().size(); ++i) {
        visit(ctx->exp(i));
        ptr = module.addGetElemPtrInst(ptr, new IntegerConstant(0), currentRet,
                                       "ret");
      }
    }
    if (ptr->getType()->getTypeTag() != TypeTag::TT_ARRAY) {
      Value* realValue = module.addLoadInst(ptr, "int");
      currentRet = realValue;
    } else {
      currentRet = ptr;
    }
    return nullptr;
  }

  antlrcpp::Any visitStmtAssign(
      SysYParserParser::StmtAssignContext* ctx) override {
    auto* lVal =
        dynamic_cast<SysYParserParser::LValSingleContext*>(ctx->lVal());
    if (lVal != nullptr) {
      Value* addr = current->get(lVal->Identifier()->getText());
      visit(ctx->exp());
      module.addStoreInst(currentRet, addr);
    } else {
      auto* lVal =
          dynamic_cast<SysYParserParser::LValArrayContext*>(ctx->lVal());
      Value* ptr = current->get(lVal->Identifier()->getText());
      if (ptr->getValueTag() == ValueTag::VT_ARG) {
        visit(lVal->exp(0));
        ptr = module.addGetElemPtrInst(ptr, currentRet, "ret");
        for (int i = 1; i < lVal->exp().size(); ++i) {
          visit(lVal->exp(i));
          ptr = module.addGetElemPtrInst(ptr, new IntegerConstant(0),
                                         currentRet, "ret");
        }
      } else {
        for (int i = 0; i < lVal->exp().size(); ++i) {
          visit(lVal->exp(i));
          ptr = module.addGetElemPtrInst(ptr, new IntegerConstant(0),
                                         currentRet, "ret");
        }
      }
      visit(ctx->exp());
      module.addStoreInst(currentRet, ptr);
    }
    return nullptr;
  }

  antlrcpp::Any visitUnaryExpUnary(
      SysYParserParser::UnaryExpUnaryContext* ctx) override {
    // TODO: unaryOp == !, swap trueBlock and falseBlock
    visit(ctx->unaryExp());
    if (ctx->unaryOp()->getText() == "-") {
      if (dynamic_cast<Constant*>(currentRet)) {
        // is a constant
        if (currentRet->isa(VT_INTCONST)) {
          int int_val = ((IntegerConstant*)currentRet)->getValue();
          currentRet = IntegerConstant::getConstInt(-int_val);
        } else {
          float float_val = ((FloatConstant*)currentRet)->getValue();
          currentRet = FloatConstant::getConstFloat(-float_val);
        }
      } else {
        if (currentRet->getType()->getTypeTag() == TT_INT32) {
          IntegerConstant* zero = IntegerConstant::getConstInt(0);
          currentRet = module.addBinaryOpInst(SUB, zero, currentRet, "minus");
        } else {
          FloatConstant* zero = FloatConstant::getConstFloat(0);
          currentRet = module.addBinaryOpInst(FSUB, zero, currentRet, "minus");
        }
      }
    }
    return nullptr;
  }

  antlrcpp::Any visitIntOctConst(
      SysYParserParser::IntOctConstContext* ctx) override {
    int val = stoi(ctx->getText());
    Value* num = IntegerConstant::getConstInt(val);
    currentRet = num;
    return nullptr;
  }
  antlrcpp::Any visitIntDecConst(
      SysYParserParser::IntDecConstContext* ctx) override {
    int val = stoi(ctx->getText());
    Value* num = IntegerConstant::getConstInt(val);
    currentRet = num;
    return nullptr;
  }

 private:
  void buildVariable(string name, Value* val);
  void buildVariable(string name, Type* type);
};

void MySysYParserVisitor::buildVariable(std::string name, Value* val) {
  if (current->parent == nullptr) {
    GlobalVariable* gv =
        module.addGlobalVariable(val->getType(), (Constant*)val, name);
    current->put(name, gv);
    currentRet = gv;
  } else {
    AllocaInst* inst = module.addAllocaInst(val->getType(), name + ".addr");
    module.addStoreInst(val, inst);
    current->put(name, inst);
    currentRet = inst;
  }
}

void MySysYParserVisitor::buildVariable(std::string name, Type* type) {
  if (current->parent == nullptr) {
    Value* md = module.addGlobalVariable(type, name);
    current->put(name, md);
  } else {
    AllocaInst* inst = module.addAllocaInst(type, name + ".addr");
    current->put(name, inst);
  }
}

void MySysYParserVisitor::setModule(ANTPIE::Module m) { module = m; }

MySysYParserVisitor::MySysYParserVisitor(VariableTable* current)
    : current(current) {}

void MySysYParserVisitor::setArrayInitVal(Value* arr,
                                          ArrayConstant* arrayValue) {
  if (arr->isa(VT_GLOBALVAR)) {
    // Global variable
    GlobalVariable* garr = dynamic_cast<GlobalVariable*>(arr);
    garr->setInitValue(arrayValue);
    return;
  }
  // local array
  Value* addr = arr;
  Value* zero = new IntegerConstant(0);
  std::function<void(ArrayConstant*)> arrayInitDfs =
      [&](ArrayConstant* arrayConst) {
        ArrayType* arrayType = (ArrayType*)arrayConst->getType();
        int len = arrayType->getLen();
        auto* elemMap = arrayConst->getElementMap();

        if (arrayType->getElemType()->getTypeTag() != TT_ARRAY) {
          for (int loc = 0; loc < len; loc++) {
            auto it = elemMap->find(loc);
            Value* elem;
            if (it == elemMap->end()) {
              elem = arrayType->getElemType()->getZeroInit();
            } else {
              elem = it->second;
            }
            auto finalAddr = module.addGetElemPtrInst(
                addr, zero, new IntegerConstant(loc), "arr.faddr");
            module.addStoreInst(elem, finalAddr);
          }
        } else {
          Value* oldAddr = addr;
          for (int loc = 0; loc < len; loc++) {
            auto it = elemMap->find(loc);
            Value* elem;
            if (it == elemMap->end()) {
              elem = arrayType->getElemType()->getZeroInit();
            } else {
              elem = it->second;
            }
            addr = module.addGetElemPtrInst(
                oldAddr, zero, IntegerConstant::getConstInt(loc), "arr.addr");
            arrayInitDfs((ArrayConstant*)elem);
          }
          addr = oldAddr;
        }
      };
  arrayInitDfs(arrayValue);
}

#endif  // ANTPIE_MYSYSYPARSERVISITOR_H
