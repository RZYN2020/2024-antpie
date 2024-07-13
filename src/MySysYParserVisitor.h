//
// Created by yanayn on 7/10/24.
//

#ifndef ANTPIE_MYSYSYPARSERVISITOR_H
#define ANTPIE_MYSYSYPARSERVISITOR_H


#include "SysYParserBaseVisitor.h"
#include "antlr4-runtime.h"
#include "Type.hh"
#include "Constant.hh"
#include "VariableTable.h"
#include "Module.hh"

#define any antlrcpp::Any

using std::vector;
using std::stoi;
using std::string;
using ANTPIE::Module;

class MySysYParserVisitor: SysYParserBaseVisitor {
public:
    void setModule(Module m);
    Module module;
    VariableTable current;
    Type* currDefType;//当前的定义的类型，在多重定义的时候使用
    Value* currentRet;
    Type* currentRetType;
    antlrcpp::Any visitProgram(SysYParserParser::ProgramContext *ctx) override{
        current=VariableTable(nullptr);
        antlr4::tree::AbstractParseTreeVisitor::visit(ctx->compUnit());
    }

    antlrcpp::Any visitInitLVal(SysYParserParser::InitLValContext *ctx) override{
        int dimesion = ctx->constExp().size();
        Type* t = currDefType;
        for (int i = dimesion-1; i >=0 ; i--) {
            antlr4::tree::AbstractParseTreeVisitor::visit(ctx->constExp(i));
            t = new ArrayType(stoi(currentRet->toString()),t);
        }
        currentRetType = t;
    }
    antlrcpp::Any visitConstDefSingle(SysYParserParser::ConstDefSingleContext *ctx) override{
        string varName = ctx->Identifier()->getText();
        antlr4::tree::AbstractParseTreeVisitor::visit(ctx->constInitVal());

    }

private:
    void buildVariable(string name,Value* val);


};

void MySysYParserVisitor::buildVariable(std::string name, Value *val) {
if(current.parent== nullptr){
    if(val== nullptr){
        module.addGlobalVariable(val->getType(),name);
    }else{
        module.addGlobalVariable(val->getType(),(Constant*)val,name);
    }
}else{

}
}

void MySysYParserVisitor::setModule(ANTPIE::Module m) {
    module=m;
}


#endif //ANTPIE_MYSYSYPARSERVISITOR_H
