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

typedef struct whileBlockInfo{
public:
    BasicBlock* condBlock;
    BasicBlock* exitBlock;
    whileBlockInfo(BasicBlock* cond,BasicBlock* exit){
        this->condBlock = cond;
        this->exitBlock = exit;
    };
}WhileBlockInfo;




using std::vector;
using std::stoi;
using std::string;
using ANTPIE::Module;
using std::stof;
using std::stack;

class MySysYParserVisitor: SysYParserBaseVisitor {
public:
    MySysYParserVisitor(VariableTable current);

    void setModule(Module m);
    Module module;
    VariableTable current;
    Type* currDefType;//当前的定义的类型，在多重定义的时候使用
    Value* currentRet;
    Type* currentRetType;
    Type* functionRetType;
    BasicBlock* trueBasicBlock;
    BasicBlock* falseBasicBlock;
    stack<WhileBlockInfo *> loopLayer;
    Function* currentIn;
    map<string,OpTag> Integer_Operator_OpTag_Table;
    map<string,OpTag> F_Operator_OpTag_Table;
    map<string,OpTag> Logic_Operator_OpTag_Table;
    antlrcpp::Any visitProgram(SysYParserParser::ProgramContext *ctx) override{
        Integer_Operator_OpTag_Table = map<string,OpTag>();
        Logic_Operator_OpTag_Table=map<string,OpTag>();
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
        F_Operator_OpTag_Table = map<string,OpTag>();
        F_Operator_OpTag_Table["+"] = OpTag::FADD;
        F_Operator_OpTag_Table["-"] = OpTag::FSUB;
        F_Operator_OpTag_Table["*"] = OpTag::FMUL;
        F_Operator_OpTag_Table["/"] = OpTag::FDIV;
        F_Operator_OpTag_Table["%"] = OpTag::FREM;
        F_Operator_OpTag_Table[">"] = OpTag::OGT;
        F_Operator_OpTag_Table["<"] = OpTag::OLT;
        F_Operator_OpTag_Table[">="] = OpTag::OGE;
        F_Operator_OpTag_Table["<="] = OpTag::OLE;
        loopLayer=stack<WhileBlockInfo *>();
        current=VariableTable(nullptr);
        visit(ctx->compUnit());
        functionRetType= nullptr;
    }
    void buildCondition(){
        Int1Type* intType = dynamic_cast<Int1Type*>(currentRet->getType());
        if(intType!= nullptr){
            currentRet = module.addIcmpInst(OpTag::NE,currentRet,new IntegerConstant(0), "icmp eq");
        }
        module.addBranchInst(currentRet, trueBasicBlock, falseBasicBlock);
    }


    antlrcpp::Any visitInitLVal(SysYParserParser::InitLValContext *ctx) override{
        int dimesion = ctx->constExp().size();
        Type* t = currDefType;
        for (int i = dimesion-1; i >=0 ; i--) {
            visit(ctx->constExp(i));
            t = new ArrayType(stoi(currentRet->toString()),t);
        }
        currentRetType = t;
    }
    antlrcpp::Any visitConstDefSingle(SysYParserParser::ConstDefSingleContext *ctx) override{
        string varName = ctx->Identifier()->getText();
        visit(ctx->constInitVal());
        if(currDefType->getTypeTag()==TT_FLOAT){
            float f = stof(currentRet->toString());
            buildVariable(varName, new FloatConstant(f));
        }else{
            int i = stoi(currentRet->toString());
            buildVariable(varName,new IntegerConstant(i));
        }
    }
    antlrcpp::Any visitConstDefArray(SysYParserParser::ConstDefArrayContext *ctx) override{
        visit(ctx->initLVal());
        //currentRetType=Array.type
        visit(ctx->constInitVal());
        //currentRet = constInitVal.Value
        buildVariable(ctx->initLVal()->Identifier()->getText(),currentRet);
    }

    antlrcpp::Any visitVarDefSingle(SysYParserParser::VarDefSingleContext *ctx) override{
        string varName = ctx->Identifier()->getText();
        buildVariable(varName,currentRetType);
    }
    antlrcpp::Any visitVarDefArray(SysYParserParser::VarDefArrayContext *ctx) override{
        antlr4::tree::AbstractParseTreeVisitor::visit(ctx->initLVal());
        buildVariable(ctx->initLVal()->Identifier()->getText(),currentRetType);
    }
    antlrcpp::Any visitVarDefSingleInitVal(SysYParserParser::VarDefSingleInitValContext *ctx) override{
        string varName = ctx->Identifier()->getText();
        visit(ctx->initVal());
        if(currDefType->getTypeTag()==TT_FLOAT){
            float f = stof(currentRet->toString());
            buildVariable(varName, new FloatConstant(f));
        }else{
            int i = stoi(currentRet->toString());
            buildVariable(varName,new IntegerConstant(i));
        }
    }
    antlrcpp::Any visitVarDefArrayInitVal(SysYParserParser::VarDefArrayInitValContext *ctx) override{
        visit(ctx->initLVal());
        //currentRetType=Array.type
        visit(ctx->initVal());
        //currentRet = constInitVal.Value
        buildVariable(ctx->initLVal()->Identifier()->getText(),currentRet);
    }
    antlrcpp::Any visitConstDecl(SysYParserParser::ConstDeclContext *ctx) override{
        if((ctx->bType()->getText().compare((char*)'int'))==0){
            currDefType = new Int32Type();
        }else{
            currDefType = new FloatType();
        }
        for (SysYParserParser::ConstDefContext* c:ctx->constDef()) {
            visit(c);
        }
    }
    antlrcpp::Any visitVarDecl(SysYParserParser::VarDeclContext *ctx) override{
        if((ctx->bType()->getText().compare("int"))==0){
            currDefType = new Int32Type();
        }else{
            currDefType = new FloatType();
        }
        for (SysYParserParser::VarDefContext* c:ctx->varDef()) {
            visit(c);
        }
    }
    //TODO: SysYParserParser::visitConstInitValContext()等同类的模块
    antlrcpp::Any visitFuncDef(SysYParserParser::FuncDefContext *ctx) override{

        if(ctx->funcType()->getText().compare("int")){
            functionRetType=new Int32Type();
        }else if(ctx->funcType()->getText().compare("float")){
            functionRetType=new FloatType();
        }else{
            functionRetType=new VoidType();
        }
        VariableTable newScope = VariableTable(current);
        current=newScope;
        vector<Argument*> args= vector<Argument*>();
        if(ctx->funcFParams()!=nullptr) {
            //在函数参数定义范围内
            for (SysYParserParser::FuncFParamContext *co: ctx->funcFParams()->funcFParam()) {
                SysYParserParser::FuncFParamSingleContext *st = dynamic_cast<SysYParserParser::FuncFParamSingleContext *>(co);
                if (st == nullptr) {
                    SysYParserParser::FuncFParamArrayContext *ac = dynamic_cast<SysYParserParser::FuncFParamArrayContext *>(co);
                    Type *bt;
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
                    Type *prt = new PointerType(bt);
                    Argument *argu = new Argument(ac->Identifier()->getText(), prt);
                    args.push_back(argu);
                    current.put(ac->Identifier()->getText(), argu);
                } else {
                    string nm = st->Identifier()->getText();
                    if (st->bType()->getText().compare("int") == 0) {
                        Argument *argu = new Argument(nm, Type::getInt32Type());
                        args.push_back(argu);
                        current.put(nm, argu);
                    } else {
                        Argument *argu = new Argument(nm, Type::getFloatType());
                        args.push_back(argu);
                        current.put(nm, argu);
                    }
                }
            }
        }else{
            Type * rt;
            if(ctx->funcType()->getText()=="int"){
                rt=Type::getInt32Type();
            }else if(ctx->funcType()->getText()=="float"){
                rt = Type::getFloatType();
            }else{
                rt = Type::getVoidType();
            }
            FuncType* funcType = Type::getFuncType(rt);
            Function* mFunction = module.addFunction(funcType, ctx->Identifier()->getText());
            currentIn=mFunction;
            BasicBlock* basicBlock = module.addBasicBlock(mFunction, "entry");
            module.setCurrBasicBlock(basicBlock);
            for (SysYParserParser::BlockItemContext *bc:ctx->block()->blockItem()) {
                visit(bc);
            }
            current = current.parent;
            return nullptr;
        }
        Type * rt;
        if(ctx->funcType()->getText()=="int"){
            rt=Type::getInt32Type();
        }else if(ctx->funcType()->getText()=="float"){
            rt = Type::getFloatType();
        }else{
            rt = Type::getVoidType();
        }
        FuncType* funcType = Type::getFuncType(rt,args);
        Function* mFunction = module.addFunction(funcType, ctx->Identifier()->getText());
        currentIn=mFunction;
        BasicBlock* basicBlock = module.addBasicBlock(mFunction, "entry");
        module.setCurrBasicBlock(basicBlock);
        for (SysYParserParser::BlockItemContext *bc:ctx->block()->blockItem()) {
            visit(bc);
        }
        current = current.parent;
        //退出函数参数定义范围内
    }




    antlrcpp::Any visitBlock(SysYParserParser::BlockContext *ctx) override{
        VariableTable scope = VariableTable(current);
        current=scope;
        for (SysYParserParser::BlockItemContext* c:ctx->blockItem()) {
            visit(c);
        }
        current=current.parent;
    }

    antlrcpp::Any visitStmtCond(SysYParserParser::StmtCondContext *ctx) override {
        BasicBlock *trueBlock = module.addBasicBlock(currentIn, "if.true");
        BasicBlock *falseBlock = module.addBasicBlock(currentIn, "if.false");
        BasicBlock *exitBlock;
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




    antlrcpp::Any visitStmtWhile(SysYParserParser::StmtWhileContext *ctx) override{
        BasicBlock* condBlock = module.addBasicBlock(currentIn, "while.begin");
        BasicBlock* exitBlock = module.addBasicBlock(currentIn, "while.end");
        BasicBlock* bodyBlock = module.addBasicBlock(currentIn, "while.loop");
        module.addJumpInst(condBlock);
        module.setCurrBasicBlock(condBlock);
        trueBasicBlock = bodyBlock;
        falseBasicBlock = exitBlock;
        visit(ctx->cond());
        loopLayer.push(new whileBlockInfo(condBlock,exitBlock));
        module.setCurrBasicBlock(bodyBlock);
        visit(ctx->stmt());
        module.addJumpInst(condBlock);
        module.setCurrBasicBlock(exitBlock);
        loopLayer.pop();
        return nullptr;
    }



    antlrcpp::Any visitStmtBreak(SysYParserParser::StmtBreakContext *ctx) override{
        module.addJumpInst(loopLayer.top()->exitBlock);
        module.addBasicBlock(currentIn,"after_while_break");
    }

    antlrcpp::Any visitStmtContinue(SysYParserParser::StmtContinueContext *ctx) override{
        module.addJumpInst(loopLayer.top()->condBlock);
        module.addBasicBlock(currentIn,"after_while_continue");
    }
    antlrcpp::Any visitStmtReturn(SysYParserParser::StmtReturnContext *ctx) override{
        if(ctx->exp()!= nullptr){
            visit(ctx->exp());
            module.addReturnInst(currentRet);
        }else{
            module.addReturnInst();
        }
    }
    antlrcpp::Any visitLOrExp(SysYParserParser::LOrExpContext *ctx) override{
        BasicBlock* falseBlock = falseBasicBlock;
        for(int i=0;i<ctx->lAndExp().size();i++){
            if(i==ctx->lAndExp().size()-1){
                falseBasicBlock =falseBlock;
                visit(ctx->lAndExp(i));
            } else{
                BasicBlock* nextBlock = module.addBasicBlock(currentIn,"OR COND");
                falseBasicBlock = nextBlock;
                visit(ctx->lAndExp(i));
                module.setCurrBasicBlock(nextBlock);
            }
        }
        return nullptr;
    }
    antlrcpp::Any visitLAndExp(SysYParserParser::LAndExpContext *ctx) override{
        BasicBlock* trueBlock =trueBasicBlock;
        for(int i=0;i<ctx->eqExp().size();i++){
            if(i==ctx->eqExp().size()-1){
                trueBasicBlock = trueBlock;
                visit(ctx->eqExp(i));
                buildCondition();
            }
            else{
                BasicBlock* nextBlock= module.addBasicBlock(currentIn,"AND COND");
                trueBasicBlock =nextBlock;
                visit(ctx->eqExp(i));
                buildCondition();
                module.setCurrBasicBlock(nextBlock);
            }
        }
        return nullptr;
    }

    antlrcpp::Any visitUnaryExpFuncR(SysYParserParser::UnaryExpFuncRContext *ctx) override{
        vector<Value*> args = vector<Value*>();
        for (SysYParserParser::FuncRParamContext* c:ctx->funcRParams()->funcRParam()) {
            visit(c);
            args.push_back(currentRet);
        }
        currentRet=module.addCallInst((Function*)current.get(ctx->Identifier()->getText()),args,"call"+ctx
        ->Identifier()->getText());
    }


    antlrcpp::Any visitAddExp(SysYParserParser::AddExpContext *ctx) override{
        visit(ctx->left);
        Value* l_Val = currentRet;
        Value* r_Val;
        if(l_Val->isPointer()){
        LoadInst* li = module.addLoadInst(l_Val,"lval");
        l_Val=li;
        }
        for (int i = 0; i < ctx->op.size(); ++i) {
            visit(ctx->right[i]);
            r_Val=currentRet;
            if(r_Val->isPointer()){
               r_Val = module.addLoadInst(r_Val,"rval");
            }
            OpTag t;
            if((l_Val->getType()->getTypeTag()==TypeTag::TT_FLOAT)||(r_Val->getType()->getTypeTag()==TypeTag::TT_FLOAT)){
                t = F_Operator_OpTag_Table[ctx->op[i]->getText()];
                if(l_Val->getType()->getTypeTag()==TypeTag::TT_INT32){
                    l_Val = module.addSitofpInst(l_Val,"lval");
                }
                if(r_Val->getType()->getTypeTag()==TypeTag::TT_INT32){
                    r_Val = module.addSitofpInst(r_Val,"lval");
                }
                l_Val = module.addBinaryOpInst(t,l_Val,r_Val,"addres");
            }else{
                t = Integer_Operator_OpTag_Table[ctx->op[i]->getText()];
                l_Val = module.addBinaryOpInst(t,l_Val,r_Val,"addres");
            }
        }
        currentRet=l_Val;
    }
    antlrcpp::Any visitMulExp(SysYParserParser::MulExpContext *ctx) override{
        visit(ctx->left);
        Value* l_Val = currentRet;
        Value* r_Val;
        if(l_Val->isPointer()){
            LoadInst* li = module.addLoadInst(l_Val,"lval");
            l_Val=li;
        }
        for (int i = 0; i < ctx->op.size(); ++i) {
            visit(ctx->right[i]);
            r_Val=currentRet;
            if(r_Val->isPointer()){
                r_Val = module.addLoadInst(r_Val,"rval");
            }
            OpTag t;
            if((l_Val->getType()->getTypeTag()==TypeTag::TT_FLOAT)||(r_Val->getType()->getTypeTag()==TypeTag::TT_FLOAT)){
                t = F_Operator_OpTag_Table[ctx->op[i]->getText()];
                if(l_Val->getType()->getTypeTag()==TypeTag::TT_INT32){
                    l_Val = module.addSitofpInst(l_Val,"lval");
                }
                if(r_Val->getType()->getTypeTag()==TypeTag::TT_INT32){
                    r_Val = module.addSitofpInst(r_Val,"lval");
                }
                l_Val = module.addBinaryOpInst(t,l_Val,r_Val,"addres");
            }else{
                t = Integer_Operator_OpTag_Table[ctx->op[i]->getText()];
                l_Val = module.addBinaryOpInst(t,l_Val,r_Val,"addres");
            }
        }
        currentRet=l_Val;
    }

    antlrcpp::Any visitRelExp(SysYParserParser::RelExpContext *ctx) override{
        visit(ctx->left);
        Value* l_Val = currentRet;
        Value* r_Val;
        if(l_Val->isPointer()){
            LoadInst* li = module.addLoadInst(l_Val,"lval");
            l_Val=li;
        }
        for (int i = 0; i < ctx->op.size(); ++i) {
            visit(ctx->right[i]);
            r_Val=currentRet;
            if(r_Val->isPointer()){
                r_Val = module.addLoadInst(r_Val,"rval");
            }
            OpTag t;
            if((l_Val->getType()->getTypeTag()==TypeTag::TT_FLOAT)||(r_Val->getType()->getTypeTag()==TypeTag::TT_FLOAT)){
                t = F_Operator_OpTag_Table[ctx->op[i]->getText()];
                if(l_Val->getType()->getTypeTag()==TypeTag::TT_INT32){
                    l_Val = module.addSitofpInst(l_Val,"lval");
                }else if(l_Val->getType()->getTypeTag()==TypeTag::TT_INT1){
                    l_Val = module.addZextInst(l_Val,Type::getInt32Type(),"lVal");
                    l_Val = module.addSitofpInst(l_Val,"lval");
                }
                if(r_Val->getType()->getTypeTag()==TypeTag::TT_INT32){
                    r_Val = module.addSitofpInst(r_Val,"rval");
                }else if(r_Val->getType()->getTypeTag()==TypeTag::TT_INT1){
                    r_Val = module.addZextInst(r_Val,Type::getInt32Type(),"rVal");
                    r_Val = module.addSitofpInst(r_Val,"rval");
                }
                if(i>0){
                    l_Val = module.addZextInst(l_Val,Type::getInt32Type(),"lval");
                }
                l_Val = module.addBinaryOpInst(t,l_Val,r_Val,"addres");
            }else{
                t = Integer_Operator_OpTag_Table[ctx->op[i]->getText()];
                if(i>0){
                    l_Val = module.addZextInst(l_Val,Type::getInt32Type(),"lval");
                }
                l_Val = module.addBinaryOpInst(t,l_Val,r_Val,"addres");
            }
        }
        currentRet=l_Val;
    }

    antlrcpp::Any visitEqExp(SysYParserParser::EqExpContext *ctx) override{
        visit(ctx->left);
        Value* l_Val = currentRet;
        Value* r_Val;
        if(l_Val->isPointer()){
            LoadInst* li = module.addLoadInst(l_Val,"lval");
            l_Val=li;
        }
        for (int i = 0; i < ctx->op.size(); ++i) {
            visit(ctx->right[i]);
            r_Val=currentRet;
            if(r_Val->isPointer()){
                r_Val = module.addLoadInst(r_Val,"rval");
            }
            OpTag t=Logic_Operator_OpTag_Table[ctx->op[i]->getText()];
            if((l_Val->getType()->getTypeTag()==TypeTag::TT_FLOAT)||(r_Val->getType()->getTypeTag()==TypeTag::TT_FLOAT)){
                if(l_Val->getType()->getTypeTag()==TypeTag::TT_INT32){
                    l_Val = module.addSitofpInst(l_Val,"lval");
                }else if(l_Val->getType()->getTypeTag()==TypeTag::TT_INT1){
                    l_Val = module.addZextInst(l_Val,Type::getInt32Type(),"lVal");
                    l_Val = module.addSitofpInst(l_Val,"lval");
                }
                if(r_Val->getType()->getTypeTag()==TypeTag::TT_INT32){
                    r_Val = module.addSitofpInst(r_Val,"rval");
                }else if(r_Val->getType()->getTypeTag()==TypeTag::TT_INT1){
                    r_Val = module.addZextInst(r_Val,Type::getInt32Type(),"rVal");
                    r_Val = module.addSitofpInst(r_Val,"rval");
                }
                if(i>0){
                    l_Val = module.addZextInst(l_Val,Type::getInt32Type(),"lval");
                }
                l_Val = module.addBinaryOpInst(t,l_Val,r_Val,"addres");
            }else{
                if(i>0){
                    l_Val = module.addZextInst(l_Val,Type::getInt32Type(),"lval");
                }
                l_Val = module.addBinaryOpInst(t,l_Val,r_Val,"addres");
            }
        }
        currentRet=l_Val;
    }
    antlrcpp::Any visitLValSingle(SysYParserParser::LValSingleContext *ctx) override{
        Value* get = current.get(ctx->getText());
        if(get->isPointer()){
            currentRet = module.addLoadInst(get,"get");
        }else{
            currentRet = get;
        }
    }

    antlrcpp::Any visitLValArray(SysYParserParser::LValArrayContext *ctx) override{
        Value* ptr = current.get(ctx->Identifier()->getText());
        if(ptr->getValueTag()==ValueTag::VT_ARG){
            visit(ctx->exp(0));
            ptr = module.addGetElemPtrInst(ptr,currentRet,"ret");
            for (int i = 1; i < ctx->exp().size(); ++i) {
                visit(ctx->exp(i));
                ptr = module.addGetElemPtrInst(ptr,new IntegerConstant(0),currentRet,"ret");
            }
        }else{
            for (int i = 0; i < ctx->exp().size(); ++i) {
                visit(ctx->exp(i));
                ptr = module.addGetElemPtrInst(ptr,new IntegerConstant(0),currentRet,"ret");
            }
        }
        if(ptr->getType()->getTypeTag()!=TypeTag::TT_ARRAY){
            Value * realValue = module.addLoadInst(ptr,"int");
            currentRet = realValue;
        }else {
            currentRet = ptr;
        }
    }

    antlrcpp::Any visitStmtAssign(SysYParserParser::StmtAssignContext *ctx) override{
        auto* lVal = dynamic_cast<SysYParserParser::LValSingleContext*>(ctx->lVal());
        if(lVal!= nullptr){
            Value* addr = current.get(lVal->Identifier()->getText());
            visit(ctx->exp());
            module.addStoreInst(currentRet,addr);
        }else{
            auto* lVal = dynamic_cast<SysYParserParser::LValArrayContext*>(ctx->lVal());
            Value* ptr = current.get(lVal->Identifier()->getText());
            if(ptr->getValueTag()==ValueTag::VT_ARG){
                visit(lVal->exp(0));
                ptr = module.addGetElemPtrInst(ptr,currentRet,"ret");
                for (int i = 1; i < lVal->exp().size(); ++i) {
                    visit(lVal->exp(i));
                    ptr = module.addGetElemPtrInst(ptr,new IntegerConstant(0),currentRet,"ret");
                }
            }else{
                for (int i = 0; i < lVal->exp().size(); ++i) {
                    visit(lVal->exp(i));
                    ptr = module.addGetElemPtrInst(ptr,new IntegerConstant(0),currentRet,"ret");
                }
            }
            visit(ctx->exp());
            module.addStoreInst(currentRet,ptr);
        }
    }




private:
    void buildVariable(string name,Value* val);
    void buildVariable(string name,Type* type);


};

void MySysYParserVisitor::buildVariable(std::string name, Value *val) {
if(current.parent== nullptr){
        GlobalVariable* gv=module.addGlobalVariable(val->getType(),(Constant*)val,name);
        current.put(name,gv);
        currentRet=gv;
}else{
    AllocaInst* inst = module.addAllocaInst(val->getType(),name+".addr");
    module.addStoreInst(val,inst);
    current.put(name,inst);
    currentRet=inst;
}

}

void MySysYParserVisitor::buildVariable(std::string name,Type* type) {
    if(current.parent== nullptr){
        Value* md =module.addGlobalVariable(type,name);
        current.put(name,md);
    }else{
        AllocaInst* inst = module.addAllocaInst(type,name+".addr");
        current.put(name,inst);
    }
}

void MySysYParserVisitor::setModule(ANTPIE::Module m) {
    module=m;
}

MySysYParserVisitor::MySysYParserVisitor(VariableTable current) : current(current) {


}


#endif //ANTPIE_MYSYSYPARSERVISITOR_H
