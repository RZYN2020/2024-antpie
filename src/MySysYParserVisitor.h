//
// Created by yanayn on 7/10/24.
//

#ifndef ANTPIE_MYSYSYPARSERVISITOR_H
#define ANTPIE_MYSYSYPARSERVISITOR_H


#include "SysYParserBaseVisitor.h"
#include "antlr4-runtime.h"
#define any antlrcpp::Any
using std::vector;

class MySysYParserVisitor: SysYParserBaseVisitor {
private:
    antlrcpp::Any visitInitLVal(SysYParserParser::InitLValContext *ctx) override{

    }


};


#endif //ANTPIE_MYSYSYPARSERVISITOR_H
