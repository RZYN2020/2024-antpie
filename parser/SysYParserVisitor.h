
// Generated from ./parser/SysYParser.g4 by ANTLR 4.9.3

#pragma once


#include "antlr4-runtime.h"
#include "SysYParserParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by SysYParserParser.
 */
class  SysYParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by SysYParserParser.
   */
    virtual antlrcpp::Any visitProgram(SysYParserParser::ProgramContext *context) = 0;

    virtual antlrcpp::Any visitCompUnit(SysYParserParser::CompUnitContext *context) = 0;

    virtual antlrcpp::Any visitDecl(SysYParserParser::DeclContext *context) = 0;

    virtual antlrcpp::Any visitConstDecl(SysYParserParser::ConstDeclContext *context) = 0;

    virtual antlrcpp::Any visitBType(SysYParserParser::BTypeContext *context) = 0;

    virtual antlrcpp::Any visitConstDefSingle(SysYParserParser::ConstDefSingleContext *context) = 0;

    virtual antlrcpp::Any visitConstDefArray(SysYParserParser::ConstDefArrayContext *context) = 0;

    virtual antlrcpp::Any visitConstInitValSingle(SysYParserParser::ConstInitValSingleContext *context) = 0;

    virtual antlrcpp::Any visitConstInitValArray(SysYParserParser::ConstInitValArrayContext *context) = 0;

    virtual antlrcpp::Any visitVarDecl(SysYParserParser::VarDeclContext *context) = 0;

    virtual antlrcpp::Any visitVarDefSingle(SysYParserParser::VarDefSingleContext *context) = 0;

    virtual antlrcpp::Any visitVarDefArray(SysYParserParser::VarDefArrayContext *context) = 0;

    virtual antlrcpp::Any visitVarDefSingleInitVal(SysYParserParser::VarDefSingleInitValContext *context) = 0;

    virtual antlrcpp::Any visitVarDefArrayInitVal(SysYParserParser::VarDefArrayInitValContext *context) = 0;

    virtual antlrcpp::Any visitInitLVal(SysYParserParser::InitLValContext *context) = 0;

    virtual antlrcpp::Any visitInitValSingle(SysYParserParser::InitValSingleContext *context) = 0;

    virtual antlrcpp::Any visitInitValArray(SysYParserParser::InitValArrayContext *context) = 0;

    virtual antlrcpp::Any visitFuncDef(SysYParserParser::FuncDefContext *context) = 0;

    virtual antlrcpp::Any visitFuncType(SysYParserParser::FuncTypeContext *context) = 0;

    virtual antlrcpp::Any visitFuncFParams(SysYParserParser::FuncFParamsContext *context) = 0;

    virtual antlrcpp::Any visitFuncFParamSingle(SysYParserParser::FuncFParamSingleContext *context) = 0;

    virtual antlrcpp::Any visitFuncFParamArray(SysYParserParser::FuncFParamArrayContext *context) = 0;

    virtual antlrcpp::Any visitBlock(SysYParserParser::BlockContext *context) = 0;

    virtual antlrcpp::Any visitBlockItem(SysYParserParser::BlockItemContext *context) = 0;

    virtual antlrcpp::Any visitStmtAssign(SysYParserParser::StmtAssignContext *context) = 0;

    virtual antlrcpp::Any visitStmtExp(SysYParserParser::StmtExpContext *context) = 0;

    virtual antlrcpp::Any visitStmtBlock(SysYParserParser::StmtBlockContext *context) = 0;

    virtual antlrcpp::Any visitStmtCond(SysYParserParser::StmtCondContext *context) = 0;

    virtual antlrcpp::Any visitStmtWhile(SysYParserParser::StmtWhileContext *context) = 0;

    virtual antlrcpp::Any visitStmtBreak(SysYParserParser::StmtBreakContext *context) = 0;

    virtual antlrcpp::Any visitStmtContinue(SysYParserParser::StmtContinueContext *context) = 0;

    virtual antlrcpp::Any visitStmtReturn(SysYParserParser::StmtReturnContext *context) = 0;

    virtual antlrcpp::Any visitExp(SysYParserParser::ExpContext *context) = 0;

    virtual antlrcpp::Any visitCond(SysYParserParser::CondContext *context) = 0;

    virtual antlrcpp::Any visitLValSingle(SysYParserParser::LValSingleContext *context) = 0;

    virtual antlrcpp::Any visitLValArray(SysYParserParser::LValArrayContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryExpParen(SysYParserParser::PrimaryExpParenContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryExpLVal(SysYParserParser::PrimaryExpLValContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryExpNumber(SysYParserParser::PrimaryExpNumberContext *context) = 0;

    virtual antlrcpp::Any visitNumber(SysYParserParser::NumberContext *context) = 0;

    virtual antlrcpp::Any visitUnaryExpPrimaryExp(SysYParserParser::UnaryExpPrimaryExpContext *context) = 0;

    virtual antlrcpp::Any visitUnaryExpFuncR(SysYParserParser::UnaryExpFuncRContext *context) = 0;

    virtual antlrcpp::Any visitUnaryExpUnary(SysYParserParser::UnaryExpUnaryContext *context) = 0;

    virtual antlrcpp::Any visitUnaryOp(SysYParserParser::UnaryOpContext *context) = 0;

    virtual antlrcpp::Any visitFuncRParams(SysYParserParser::FuncRParamsContext *context) = 0;

    virtual antlrcpp::Any visitFuncRParam(SysYParserParser::FuncRParamContext *context) = 0;

    virtual antlrcpp::Any visitMulExp(SysYParserParser::MulExpContext *context) = 0;

    virtual antlrcpp::Any visitAddExp(SysYParserParser::AddExpContext *context) = 0;

    virtual antlrcpp::Any visitRelExp(SysYParserParser::RelExpContext *context) = 0;

    virtual antlrcpp::Any visitEqExp(SysYParserParser::EqExpContext *context) = 0;

    virtual antlrcpp::Any visitLAndExp(SysYParserParser::LAndExpContext *context) = 0;

    virtual antlrcpp::Any visitLOrExp(SysYParserParser::LOrExpContext *context) = 0;

    virtual antlrcpp::Any visitConstExp(SysYParserParser::ConstExpContext *context) = 0;

    virtual antlrcpp::Any visitIntDecConst(SysYParserParser::IntDecConstContext *context) = 0;

    virtual antlrcpp::Any visitIntOctConst(SysYParserParser::IntOctConstContext *context) = 0;

    virtual antlrcpp::Any visitIntHexConst(SysYParserParser::IntHexConstContext *context) = 0;

    virtual antlrcpp::Any visitFloatConst(SysYParserParser::FloatConstContext *context) = 0;


};

