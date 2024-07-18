
// Generated from ./parser/SysYParser.g4 by ANTLR 4.9.3

#pragma once


#include "antlr4-runtime.h"
#include "SysYParserVisitor.h"


/**
 * This class provides an empty implementation of SysYParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  SysYParserBaseVisitor : public SysYParserVisitor {
public:

  virtual antlrcpp::Any visitProgram(SysYParserParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCompUnit(SysYParserParser::CompUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDecl(SysYParserParser::DeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstDecl(SysYParserParser::ConstDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBType(SysYParserParser::BTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstDefSingle(SysYParserParser::ConstDefSingleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstDefArray(SysYParserParser::ConstDefArrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstInitValSingle(SysYParserParser::ConstInitValSingleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstInitValArray(SysYParserParser::ConstInitValArrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarDecl(SysYParserParser::VarDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarDefSingle(SysYParserParser::VarDefSingleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarDefArray(SysYParserParser::VarDefArrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarDefSingleInitVal(SysYParserParser::VarDefSingleInitValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarDefArrayInitVal(SysYParserParser::VarDefArrayInitValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitInitLVal(SysYParserParser::InitLValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitInitValSingle(SysYParserParser::InitValSingleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitInitValArray(SysYParserParser::InitValArrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncDef(SysYParserParser::FuncDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncType(SysYParserParser::FuncTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncFParams(SysYParserParser::FuncFParamsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncFParamSingle(SysYParserParser::FuncFParamSingleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncFParamArray(SysYParserParser::FuncFParamArrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBlock(SysYParserParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBlockItem(SysYParserParser::BlockItemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtAssign(SysYParserParser::StmtAssignContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtExp(SysYParserParser::StmtExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtBlock(SysYParserParser::StmtBlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtCond(SysYParserParser::StmtCondContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtWhile(SysYParserParser::StmtWhileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtBreak(SysYParserParser::StmtBreakContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtContinue(SysYParserParser::StmtContinueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtReturn(SysYParserParser::StmtReturnContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExp(SysYParserParser::ExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCond(SysYParserParser::CondContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLValSingle(SysYParserParser::LValSingleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLValArray(SysYParserParser::LValArrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryExpParen(SysYParserParser::PrimaryExpParenContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryExpLVal(SysYParserParser::PrimaryExpLValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryExpNumber(SysYParserParser::PrimaryExpNumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNumber(SysYParserParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnaryExpPrimaryExp(SysYParserParser::UnaryExpPrimaryExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnaryExpFuncR(SysYParserParser::UnaryExpFuncRContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnaryExpUnary(SysYParserParser::UnaryExpUnaryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnaryOp(SysYParserParser::UnaryOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncRParams(SysYParserParser::FuncRParamsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncRParam(SysYParserParser::FuncRParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMulExp(SysYParserParser::MulExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAddExp(SysYParserParser::AddExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRelExp(SysYParserParser::RelExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEqExp(SysYParserParser::EqExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLAndExp(SysYParserParser::LAndExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLOrExp(SysYParserParser::LOrExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstExp(SysYParserParser::ConstExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIntDecConst(SysYParserParser::IntDecConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIntOctConst(SysYParserParser::IntOctConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIntHexConst(SysYParserParser::IntHexConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFloatConst(SysYParserParser::FloatConstContext *ctx) override {
    return visitChildren(ctx);
  }


};

