
// Generated from ./parser/SysYParser.g4 by ANTLR 4.9.3


#include "SysYParserListener.h"
#include "SysYParserVisitor.h"

#include "SysYParserParser.h"


using namespace antlrcpp;
using namespace antlr4;

SysYParserParser::SysYParserParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

SysYParserParser::~SysYParserParser() {
  delete _interpreter;
}

std::string SysYParserParser::getGrammarFileName() const {
  return "SysYParser.g4";
}

const std::vector<std::string>& SysYParserParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& SysYParserParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- ProgramContext ------------------------------------------------------------------

SysYParserParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::CompUnitContext* SysYParserParser::ProgramContext::compUnit() {
  return getRuleContext<SysYParserParser::CompUnitContext>(0);
}


size_t SysYParserParser::ProgramContext::getRuleIndex() const {
  return SysYParserParser::RuleProgram;
}

void SysYParserParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void SysYParserParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}


antlrcpp::Any SysYParserParser::ProgramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitProgram(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::ProgramContext* SysYParserParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, SysYParserParser::RuleProgram);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(72);
    compUnit();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CompUnitContext ------------------------------------------------------------------

SysYParserParser::CompUnitContext::CompUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParserParser::CompUnitContext::EOF() {
  return getToken(SysYParserParser::EOF, 0);
}

std::vector<SysYParserParser::DeclContext *> SysYParserParser::CompUnitContext::decl() {
  return getRuleContexts<SysYParserParser::DeclContext>();
}

SysYParserParser::DeclContext* SysYParserParser::CompUnitContext::decl(size_t i) {
  return getRuleContext<SysYParserParser::DeclContext>(i);
}

std::vector<SysYParserParser::FuncDefContext *> SysYParserParser::CompUnitContext::funcDef() {
  return getRuleContexts<SysYParserParser::FuncDefContext>();
}

SysYParserParser::FuncDefContext* SysYParserParser::CompUnitContext::funcDef(size_t i) {
  return getRuleContext<SysYParserParser::FuncDefContext>(i);
}


size_t SysYParserParser::CompUnitContext::getRuleIndex() const {
  return SysYParserParser::RuleCompUnit;
}

void SysYParserParser::CompUnitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCompUnit(this);
}

void SysYParserParser::CompUnitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCompUnit(this);
}


antlrcpp::Any SysYParserParser::CompUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitCompUnit(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::CompUnitContext* SysYParserParser::compUnit() {
  CompUnitContext *_localctx = _tracker.createInstance<CompUnitContext>(_ctx, getState());
  enterRule(_localctx, 2, SysYParserParser::RuleCompUnit);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(78);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParserParser::T__0)
      | (1ULL << SysYParserParser::T__3)
      | (1ULL << SysYParserParser::T__4)
      | (1ULL << SysYParserParser::T__12))) != 0)) {
      setState(76);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
      case 1: {
        setState(74);
        decl();
        break;
      }

      case 2: {
        setState(75);
        funcDef();
        break;
      }

      default:
        break;
      }
      setState(80);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(81);
    match(SysYParserParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DeclContext ------------------------------------------------------------------

SysYParserParser::DeclContext::DeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::ConstDeclContext* SysYParserParser::DeclContext::constDecl() {
  return getRuleContext<SysYParserParser::ConstDeclContext>(0);
}

SysYParserParser::VarDeclContext* SysYParserParser::DeclContext::varDecl() {
  return getRuleContext<SysYParserParser::VarDeclContext>(0);
}


size_t SysYParserParser::DeclContext::getRuleIndex() const {
  return SysYParserParser::RuleDecl;
}

void SysYParserParser::DeclContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDecl(this);
}

void SysYParserParser::DeclContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDecl(this);
}


antlrcpp::Any SysYParserParser::DeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::DeclContext* SysYParserParser::decl() {
  DeclContext *_localctx = _tracker.createInstance<DeclContext>(_ctx, getState());
  enterRule(_localctx, 4, SysYParserParser::RuleDecl);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(85);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParserParser::T__0: {
        enterOuterAlt(_localctx, 1);
        setState(83);
        constDecl();
        break;
      }

      case SysYParserParser::T__3:
      case SysYParserParser::T__4: {
        enterOuterAlt(_localctx, 2);
        setState(84);
        varDecl();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstDeclContext ------------------------------------------------------------------

SysYParserParser::ConstDeclContext::ConstDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::BTypeContext* SysYParserParser::ConstDeclContext::bType() {
  return getRuleContext<SysYParserParser::BTypeContext>(0);
}

std::vector<SysYParserParser::ConstDefContext *> SysYParserParser::ConstDeclContext::constDef() {
  return getRuleContexts<SysYParserParser::ConstDefContext>();
}

SysYParserParser::ConstDefContext* SysYParserParser::ConstDeclContext::constDef(size_t i) {
  return getRuleContext<SysYParserParser::ConstDefContext>(i);
}


size_t SysYParserParser::ConstDeclContext::getRuleIndex() const {
  return SysYParserParser::RuleConstDecl;
}

void SysYParserParser::ConstDeclContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstDecl(this);
}

void SysYParserParser::ConstDeclContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstDecl(this);
}


antlrcpp::Any SysYParserParser::ConstDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::ConstDeclContext* SysYParserParser::constDecl() {
  ConstDeclContext *_localctx = _tracker.createInstance<ConstDeclContext>(_ctx, getState());
  enterRule(_localctx, 6, SysYParserParser::RuleConstDecl);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(87);
    match(SysYParserParser::T__0);
    setState(88);
    bType();
    setState(89);
    constDef();
    setState(94);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParserParser::T__1) {
      setState(90);
      match(SysYParserParser::T__1);
      setState(91);
      constDef();
      setState(96);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(97);
    match(SysYParserParser::T__2);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BTypeContext ------------------------------------------------------------------

SysYParserParser::BTypeContext::BTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::BTypeContext::getRuleIndex() const {
  return SysYParserParser::RuleBType;
}

void SysYParserParser::BTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBType(this);
}

void SysYParserParser::BTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBType(this);
}


antlrcpp::Any SysYParserParser::BTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitBType(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::BTypeContext* SysYParserParser::bType() {
  BTypeContext *_localctx = _tracker.createInstance<BTypeContext>(_ctx, getState());
  enterRule(_localctx, 8, SysYParserParser::RuleBType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(99);
    _la = _input->LA(1);
    if (!(_la == SysYParserParser::T__3

    || _la == SysYParserParser::T__4)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstDefContext ------------------------------------------------------------------

SysYParserParser::ConstDefContext::ConstDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::ConstDefContext::getRuleIndex() const {
  return SysYParserParser::RuleConstDef;
}

void SysYParserParser::ConstDefContext::copyFrom(ConstDefContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ConstDefSingleContext ------------------------------------------------------------------

tree::TerminalNode* SysYParserParser::ConstDefSingleContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

SysYParserParser::ConstInitValContext* SysYParserParser::ConstDefSingleContext::constInitVal() {
  return getRuleContext<SysYParserParser::ConstInitValContext>(0);
}

SysYParserParser::ConstDefSingleContext::ConstDefSingleContext(ConstDefContext *ctx) { copyFrom(ctx); }

void SysYParserParser::ConstDefSingleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstDefSingle(this);
}
void SysYParserParser::ConstDefSingleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstDefSingle(this);
}

antlrcpp::Any SysYParserParser::ConstDefSingleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstDefSingle(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ConstDefArrayContext ------------------------------------------------------------------

SysYParserParser::InitLValContext* SysYParserParser::ConstDefArrayContext::initLVal() {
  return getRuleContext<SysYParserParser::InitLValContext>(0);
}

SysYParserParser::ConstInitValContext* SysYParserParser::ConstDefArrayContext::constInitVal() {
  return getRuleContext<SysYParserParser::ConstInitValContext>(0);
}

SysYParserParser::ConstDefArrayContext::ConstDefArrayContext(ConstDefContext *ctx) { copyFrom(ctx); }

void SysYParserParser::ConstDefArrayContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstDefArray(this);
}
void SysYParserParser::ConstDefArrayContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstDefArray(this);
}

antlrcpp::Any SysYParserParser::ConstDefArrayContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstDefArray(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::ConstDefContext* SysYParserParser::constDef() {
  ConstDefContext *_localctx = _tracker.createInstance<ConstDefContext>(_ctx, getState());
  enterRule(_localctx, 10, SysYParserParser::RuleConstDef);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(108);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SysYParserParser::ConstDefSingleContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(101);
      match(SysYParserParser::Identifier);
      setState(102);
      match(SysYParserParser::T__5);
      setState(103);
      constInitVal();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SysYParserParser::ConstDefArrayContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(104);
      initLVal();
      setState(105);
      match(SysYParserParser::T__5);
      setState(106);
      constInitVal();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstInitValContext ------------------------------------------------------------------

SysYParserParser::ConstInitValContext::ConstInitValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::ConstInitValContext::getRuleIndex() const {
  return SysYParserParser::RuleConstInitVal;
}

void SysYParserParser::ConstInitValContext::copyFrom(ConstInitValContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ConstInitValSingleContext ------------------------------------------------------------------

SysYParserParser::ConstExpContext* SysYParserParser::ConstInitValSingleContext::constExp() {
  return getRuleContext<SysYParserParser::ConstExpContext>(0);
}

SysYParserParser::ConstInitValSingleContext::ConstInitValSingleContext(ConstInitValContext *ctx) { copyFrom(ctx); }

void SysYParserParser::ConstInitValSingleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstInitValSingle(this);
}
void SysYParserParser::ConstInitValSingleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstInitValSingle(this);
}

antlrcpp::Any SysYParserParser::ConstInitValSingleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstInitValSingle(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ConstInitValArrayContext ------------------------------------------------------------------

std::vector<SysYParserParser::ConstInitValContext *> SysYParserParser::ConstInitValArrayContext::constInitVal() {
  return getRuleContexts<SysYParserParser::ConstInitValContext>();
}

SysYParserParser::ConstInitValContext* SysYParserParser::ConstInitValArrayContext::constInitVal(size_t i) {
  return getRuleContext<SysYParserParser::ConstInitValContext>(i);
}

SysYParserParser::ConstInitValArrayContext::ConstInitValArrayContext(ConstInitValContext *ctx) { copyFrom(ctx); }

void SysYParserParser::ConstInitValArrayContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstInitValArray(this);
}
void SysYParserParser::ConstInitValArrayContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstInitValArray(this);
}

antlrcpp::Any SysYParserParser::ConstInitValArrayContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstInitValArray(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::ConstInitValContext* SysYParserParser::constInitVal() {
  ConstInitValContext *_localctx = _tracker.createInstance<ConstInitValContext>(_ctx, getState());
  enterRule(_localctx, 12, SysYParserParser::RuleConstInitVal);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(123);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParserParser::T__10:
      case SysYParserParser::T__19:
      case SysYParserParser::T__20:
      case SysYParserParser::T__21:
      case SysYParserParser::Identifier:
      case SysYParserParser::DecimalConstant:
      case SysYParserParser::OctalConstant:
      case SysYParserParser::HexadecimalConstant:
      case SysYParserParser::FloatingConstant: {
        _localctx = _tracker.createInstance<SysYParserParser::ConstInitValSingleContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(110);
        constExp();
        break;
      }

      case SysYParserParser::T__6: {
        _localctx = _tracker.createInstance<SysYParserParser::ConstInitValArrayContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(111);
        match(SysYParserParser::T__6);
        setState(120);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << SysYParserParser::T__6)
          | (1ULL << SysYParserParser::T__10)
          | (1ULL << SysYParserParser::T__19)
          | (1ULL << SysYParserParser::T__20)
          | (1ULL << SysYParserParser::T__21)
          | (1ULL << SysYParserParser::Identifier)
          | (1ULL << SysYParserParser::DecimalConstant)
          | (1ULL << SysYParserParser::OctalConstant)
          | (1ULL << SysYParserParser::HexadecimalConstant)
          | (1ULL << SysYParserParser::FloatingConstant))) != 0)) {
          setState(112);
          constInitVal();
          setState(117);
          _errHandler->sync(this);
          _la = _input->LA(1);
          while (_la == SysYParserParser::T__1) {
            setState(113);
            match(SysYParserParser::T__1);
            setState(114);
            constInitVal();
            setState(119);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
        }
        setState(122);
        match(SysYParserParser::T__7);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDeclContext ------------------------------------------------------------------

SysYParserParser::VarDeclContext::VarDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::BTypeContext* SysYParserParser::VarDeclContext::bType() {
  return getRuleContext<SysYParserParser::BTypeContext>(0);
}

std::vector<SysYParserParser::VarDefContext *> SysYParserParser::VarDeclContext::varDef() {
  return getRuleContexts<SysYParserParser::VarDefContext>();
}

SysYParserParser::VarDefContext* SysYParserParser::VarDeclContext::varDef(size_t i) {
  return getRuleContext<SysYParserParser::VarDefContext>(i);
}


size_t SysYParserParser::VarDeclContext::getRuleIndex() const {
  return SysYParserParser::RuleVarDecl;
}

void SysYParserParser::VarDeclContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVarDecl(this);
}

void SysYParserParser::VarDeclContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVarDecl(this);
}


antlrcpp::Any SysYParserParser::VarDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitVarDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::VarDeclContext* SysYParserParser::varDecl() {
  VarDeclContext *_localctx = _tracker.createInstance<VarDeclContext>(_ctx, getState());
  enterRule(_localctx, 14, SysYParserParser::RuleVarDecl);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(125);
    bType();
    setState(126);
    varDef();
    setState(131);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParserParser::T__1) {
      setState(127);
      match(SysYParserParser::T__1);
      setState(128);
      varDef();
      setState(133);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(134);
    match(SysYParserParser::T__2);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDefContext ------------------------------------------------------------------

SysYParserParser::VarDefContext::VarDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::VarDefContext::getRuleIndex() const {
  return SysYParserParser::RuleVarDef;
}

void SysYParserParser::VarDefContext::copyFrom(VarDefContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- VarDefSingleInitValContext ------------------------------------------------------------------

tree::TerminalNode* SysYParserParser::VarDefSingleInitValContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

SysYParserParser::InitValContext* SysYParserParser::VarDefSingleInitValContext::initVal() {
  return getRuleContext<SysYParserParser::InitValContext>(0);
}

SysYParserParser::VarDefSingleInitValContext::VarDefSingleInitValContext(VarDefContext *ctx) { copyFrom(ctx); }

void SysYParserParser::VarDefSingleInitValContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVarDefSingleInitVal(this);
}
void SysYParserParser::VarDefSingleInitValContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVarDefSingleInitVal(this);
}

antlrcpp::Any SysYParserParser::VarDefSingleInitValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitVarDefSingleInitVal(this);
  else
    return visitor->visitChildren(this);
}
//----------------- VarDefArrayInitValContext ------------------------------------------------------------------

SysYParserParser::InitLValContext* SysYParserParser::VarDefArrayInitValContext::initLVal() {
  return getRuleContext<SysYParserParser::InitLValContext>(0);
}

SysYParserParser::InitValContext* SysYParserParser::VarDefArrayInitValContext::initVal() {
  return getRuleContext<SysYParserParser::InitValContext>(0);
}

SysYParserParser::VarDefArrayInitValContext::VarDefArrayInitValContext(VarDefContext *ctx) { copyFrom(ctx); }

void SysYParserParser::VarDefArrayInitValContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVarDefArrayInitVal(this);
}
void SysYParserParser::VarDefArrayInitValContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVarDefArrayInitVal(this);
}

antlrcpp::Any SysYParserParser::VarDefArrayInitValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitVarDefArrayInitVal(this);
  else
    return visitor->visitChildren(this);
}
//----------------- VarDefSingleContext ------------------------------------------------------------------

tree::TerminalNode* SysYParserParser::VarDefSingleContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

SysYParserParser::VarDefSingleContext::VarDefSingleContext(VarDefContext *ctx) { copyFrom(ctx); }

void SysYParserParser::VarDefSingleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVarDefSingle(this);
}
void SysYParserParser::VarDefSingleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVarDefSingle(this);
}

antlrcpp::Any SysYParserParser::VarDefSingleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitVarDefSingle(this);
  else
    return visitor->visitChildren(this);
}
//----------------- VarDefArrayContext ------------------------------------------------------------------

SysYParserParser::InitLValContext* SysYParserParser::VarDefArrayContext::initLVal() {
  return getRuleContext<SysYParserParser::InitLValContext>(0);
}

SysYParserParser::VarDefArrayContext::VarDefArrayContext(VarDefContext *ctx) { copyFrom(ctx); }

void SysYParserParser::VarDefArrayContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVarDefArray(this);
}
void SysYParserParser::VarDefArrayContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVarDefArray(this);
}

antlrcpp::Any SysYParserParser::VarDefArrayContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitVarDefArray(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::VarDefContext* SysYParserParser::varDef() {
  VarDefContext *_localctx = _tracker.createInstance<VarDefContext>(_ctx, getState());
  enterRule(_localctx, 16, SysYParserParser::RuleVarDef);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(145);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SysYParserParser::VarDefSingleContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(136);
      match(SysYParserParser::Identifier);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SysYParserParser::VarDefArrayContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(137);
      initLVal();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<SysYParserParser::VarDefSingleInitValContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(138);
      match(SysYParserParser::Identifier);
      setState(139);
      match(SysYParserParser::T__5);
      setState(140);
      initVal();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<SysYParserParser::VarDefArrayInitValContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(141);
      initLVal();
      setState(142);
      match(SysYParserParser::T__5);
      setState(143);
      initVal();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InitLValContext ------------------------------------------------------------------

SysYParserParser::InitLValContext::InitLValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParserParser::InitLValContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

std::vector<SysYParserParser::ConstExpContext *> SysYParserParser::InitLValContext::constExp() {
  return getRuleContexts<SysYParserParser::ConstExpContext>();
}

SysYParserParser::ConstExpContext* SysYParserParser::InitLValContext::constExp(size_t i) {
  return getRuleContext<SysYParserParser::ConstExpContext>(i);
}


size_t SysYParserParser::InitLValContext::getRuleIndex() const {
  return SysYParserParser::RuleInitLVal;
}

void SysYParserParser::InitLValContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInitLVal(this);
}

void SysYParserParser::InitLValContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInitLVal(this);
}


antlrcpp::Any SysYParserParser::InitLValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitInitLVal(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::InitLValContext* SysYParserParser::initLVal() {
  InitLValContext *_localctx = _tracker.createInstance<InitLValContext>(_ctx, getState());
  enterRule(_localctx, 18, SysYParserParser::RuleInitLVal);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(147);
    match(SysYParserParser::Identifier);
    setState(152); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(148);
      match(SysYParserParser::T__8);
      setState(149);
      constExp();
      setState(150);
      match(SysYParserParser::T__9);
      setState(154); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == SysYParserParser::T__8);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InitValContext ------------------------------------------------------------------

SysYParserParser::InitValContext::InitValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::InitValContext::getRuleIndex() const {
  return SysYParserParser::RuleInitVal;
}

void SysYParserParser::InitValContext::copyFrom(InitValContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- InitValArrayContext ------------------------------------------------------------------

std::vector<SysYParserParser::InitValContext *> SysYParserParser::InitValArrayContext::initVal() {
  return getRuleContexts<SysYParserParser::InitValContext>();
}

SysYParserParser::InitValContext* SysYParserParser::InitValArrayContext::initVal(size_t i) {
  return getRuleContext<SysYParserParser::InitValContext>(i);
}

SysYParserParser::InitValArrayContext::InitValArrayContext(InitValContext *ctx) { copyFrom(ctx); }

void SysYParserParser::InitValArrayContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInitValArray(this);
}
void SysYParserParser::InitValArrayContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInitValArray(this);
}

antlrcpp::Any SysYParserParser::InitValArrayContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitInitValArray(this);
  else
    return visitor->visitChildren(this);
}
//----------------- InitValSingleContext ------------------------------------------------------------------

SysYParserParser::ExpContext* SysYParserParser::InitValSingleContext::exp() {
  return getRuleContext<SysYParserParser::ExpContext>(0);
}

SysYParserParser::InitValSingleContext::InitValSingleContext(InitValContext *ctx) { copyFrom(ctx); }

void SysYParserParser::InitValSingleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInitValSingle(this);
}
void SysYParserParser::InitValSingleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInitValSingle(this);
}

antlrcpp::Any SysYParserParser::InitValSingleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitInitValSingle(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::InitValContext* SysYParserParser::initVal() {
  InitValContext *_localctx = _tracker.createInstance<InitValContext>(_ctx, getState());
  enterRule(_localctx, 20, SysYParserParser::RuleInitVal);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(169);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParserParser::T__10:
      case SysYParserParser::T__19:
      case SysYParserParser::T__20:
      case SysYParserParser::T__21:
      case SysYParserParser::Identifier:
      case SysYParserParser::DecimalConstant:
      case SysYParserParser::OctalConstant:
      case SysYParserParser::HexadecimalConstant:
      case SysYParserParser::FloatingConstant: {
        _localctx = _tracker.createInstance<SysYParserParser::InitValSingleContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(156);
        exp();
        break;
      }

      case SysYParserParser::T__6: {
        _localctx = _tracker.createInstance<SysYParserParser::InitValArrayContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(157);
        match(SysYParserParser::T__6);
        setState(166);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << SysYParserParser::T__6)
          | (1ULL << SysYParserParser::T__10)
          | (1ULL << SysYParserParser::T__19)
          | (1ULL << SysYParserParser::T__20)
          | (1ULL << SysYParserParser::T__21)
          | (1ULL << SysYParserParser::Identifier)
          | (1ULL << SysYParserParser::DecimalConstant)
          | (1ULL << SysYParserParser::OctalConstant)
          | (1ULL << SysYParserParser::HexadecimalConstant)
          | (1ULL << SysYParserParser::FloatingConstant))) != 0)) {
          setState(158);
          initVal();
          setState(163);
          _errHandler->sync(this);
          _la = _input->LA(1);
          while (_la == SysYParserParser::T__1) {
            setState(159);
            match(SysYParserParser::T__1);
            setState(160);
            initVal();
            setState(165);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
        }
        setState(168);
        match(SysYParserParser::T__7);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncDefContext ------------------------------------------------------------------

SysYParserParser::FuncDefContext::FuncDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::FuncTypeContext* SysYParserParser::FuncDefContext::funcType() {
  return getRuleContext<SysYParserParser::FuncTypeContext>(0);
}

tree::TerminalNode* SysYParserParser::FuncDefContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

SysYParserParser::BlockContext* SysYParserParser::FuncDefContext::block() {
  return getRuleContext<SysYParserParser::BlockContext>(0);
}

SysYParserParser::FuncFParamsContext* SysYParserParser::FuncDefContext::funcFParams() {
  return getRuleContext<SysYParserParser::FuncFParamsContext>(0);
}


size_t SysYParserParser::FuncDefContext::getRuleIndex() const {
  return SysYParserParser::RuleFuncDef;
}

void SysYParserParser::FuncDefContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFuncDef(this);
}

void SysYParserParser::FuncDefContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFuncDef(this);
}


antlrcpp::Any SysYParserParser::FuncDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::FuncDefContext* SysYParserParser::funcDef() {
  FuncDefContext *_localctx = _tracker.createInstance<FuncDefContext>(_ctx, getState());
  enterRule(_localctx, 22, SysYParserParser::RuleFuncDef);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(171);
    funcType();
    setState(172);
    match(SysYParserParser::Identifier);
    setState(173);
    match(SysYParserParser::T__10);
    setState(175);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParserParser::T__3

    || _la == SysYParserParser::T__4) {
      setState(174);
      funcFParams();
    }
    setState(177);
    match(SysYParserParser::T__11);
    setState(178);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncTypeContext ------------------------------------------------------------------

SysYParserParser::FuncTypeContext::FuncTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::FuncTypeContext::getRuleIndex() const {
  return SysYParserParser::RuleFuncType;
}

void SysYParserParser::FuncTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFuncType(this);
}

void SysYParserParser::FuncTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFuncType(this);
}


antlrcpp::Any SysYParserParser::FuncTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncType(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::FuncTypeContext* SysYParserParser::funcType() {
  FuncTypeContext *_localctx = _tracker.createInstance<FuncTypeContext>(_ctx, getState());
  enterRule(_localctx, 24, SysYParserParser::RuleFuncType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(180);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParserParser::T__3)
      | (1ULL << SysYParserParser::T__4)
      | (1ULL << SysYParserParser::T__12))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncFParamsContext ------------------------------------------------------------------

SysYParserParser::FuncFParamsContext::FuncFParamsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParserParser::FuncFParamContext *> SysYParserParser::FuncFParamsContext::funcFParam() {
  return getRuleContexts<SysYParserParser::FuncFParamContext>();
}

SysYParserParser::FuncFParamContext* SysYParserParser::FuncFParamsContext::funcFParam(size_t i) {
  return getRuleContext<SysYParserParser::FuncFParamContext>(i);
}


size_t SysYParserParser::FuncFParamsContext::getRuleIndex() const {
  return SysYParserParser::RuleFuncFParams;
}

void SysYParserParser::FuncFParamsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFuncFParams(this);
}

void SysYParserParser::FuncFParamsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFuncFParams(this);
}


antlrcpp::Any SysYParserParser::FuncFParamsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncFParams(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::FuncFParamsContext* SysYParserParser::funcFParams() {
  FuncFParamsContext *_localctx = _tracker.createInstance<FuncFParamsContext>(_ctx, getState());
  enterRule(_localctx, 26, SysYParserParser::RuleFuncFParams);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(182);
    funcFParam();
    setState(187);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParserParser::T__1) {
      setState(183);
      match(SysYParserParser::T__1);
      setState(184);
      funcFParam();
      setState(189);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncFParamContext ------------------------------------------------------------------

SysYParserParser::FuncFParamContext::FuncFParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::FuncFParamContext::getRuleIndex() const {
  return SysYParserParser::RuleFuncFParam;
}

void SysYParserParser::FuncFParamContext::copyFrom(FuncFParamContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- FuncFParamArrayContext ------------------------------------------------------------------

SysYParserParser::BTypeContext* SysYParserParser::FuncFParamArrayContext::bType() {
  return getRuleContext<SysYParserParser::BTypeContext>(0);
}

tree::TerminalNode* SysYParserParser::FuncFParamArrayContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

std::vector<SysYParserParser::ExpContext *> SysYParserParser::FuncFParamArrayContext::exp() {
  return getRuleContexts<SysYParserParser::ExpContext>();
}

SysYParserParser::ExpContext* SysYParserParser::FuncFParamArrayContext::exp(size_t i) {
  return getRuleContext<SysYParserParser::ExpContext>(i);
}

SysYParserParser::FuncFParamArrayContext::FuncFParamArrayContext(FuncFParamContext *ctx) { copyFrom(ctx); }

void SysYParserParser::FuncFParamArrayContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFuncFParamArray(this);
}
void SysYParserParser::FuncFParamArrayContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFuncFParamArray(this);
}

antlrcpp::Any SysYParserParser::FuncFParamArrayContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncFParamArray(this);
  else
    return visitor->visitChildren(this);
}
//----------------- FuncFParamSingleContext ------------------------------------------------------------------

SysYParserParser::BTypeContext* SysYParserParser::FuncFParamSingleContext::bType() {
  return getRuleContext<SysYParserParser::BTypeContext>(0);
}

tree::TerminalNode* SysYParserParser::FuncFParamSingleContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

SysYParserParser::FuncFParamSingleContext::FuncFParamSingleContext(FuncFParamContext *ctx) { copyFrom(ctx); }

void SysYParserParser::FuncFParamSingleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFuncFParamSingle(this);
}
void SysYParserParser::FuncFParamSingleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFuncFParamSingle(this);
}

antlrcpp::Any SysYParserParser::FuncFParamSingleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncFParamSingle(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::FuncFParamContext* SysYParserParser::funcFParam() {
  FuncFParamContext *_localctx = _tracker.createInstance<FuncFParamContext>(_ctx, getState());
  enterRule(_localctx, 28, SysYParserParser::RuleFuncFParam);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(206);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SysYParserParser::FuncFParamSingleContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(190);
      bType();
      setState(191);
      match(SysYParserParser::Identifier);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SysYParserParser::FuncFParamArrayContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(193);
      bType();
      setState(194);
      match(SysYParserParser::Identifier);
      setState(195);
      match(SysYParserParser::T__8);
      setState(196);
      match(SysYParserParser::T__9);
      setState(203);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParserParser::T__8) {
        setState(197);
        match(SysYParserParser::T__8);
        setState(198);
        exp();
        setState(199);
        match(SysYParserParser::T__9);
        setState(205);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

SysYParserParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParserParser::BlockItemContext *> SysYParserParser::BlockContext::blockItem() {
  return getRuleContexts<SysYParserParser::BlockItemContext>();
}

SysYParserParser::BlockItemContext* SysYParserParser::BlockContext::blockItem(size_t i) {
  return getRuleContext<SysYParserParser::BlockItemContext>(i);
}


size_t SysYParserParser::BlockContext::getRuleIndex() const {
  return SysYParserParser::RuleBlock;
}

void SysYParserParser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void SysYParserParser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}


antlrcpp::Any SysYParserParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::BlockContext* SysYParserParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 30, SysYParserParser::RuleBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(208);
    match(SysYParserParser::T__6);
    setState(212);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParserParser::T__0)
      | (1ULL << SysYParserParser::T__2)
      | (1ULL << SysYParserParser::T__3)
      | (1ULL << SysYParserParser::T__4)
      | (1ULL << SysYParserParser::T__6)
      | (1ULL << SysYParserParser::T__10)
      | (1ULL << SysYParserParser::T__13)
      | (1ULL << SysYParserParser::T__15)
      | (1ULL << SysYParserParser::T__16)
      | (1ULL << SysYParserParser::T__17)
      | (1ULL << SysYParserParser::T__18)
      | (1ULL << SysYParserParser::T__19)
      | (1ULL << SysYParserParser::T__20)
      | (1ULL << SysYParserParser::T__21)
      | (1ULL << SysYParserParser::Identifier)
      | (1ULL << SysYParserParser::DecimalConstant)
      | (1ULL << SysYParserParser::OctalConstant)
      | (1ULL << SysYParserParser::HexadecimalConstant)
      | (1ULL << SysYParserParser::FloatingConstant))) != 0)) {
      setState(209);
      blockItem();
      setState(214);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(215);
    match(SysYParserParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockItemContext ------------------------------------------------------------------

SysYParserParser::BlockItemContext::BlockItemContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::DeclContext* SysYParserParser::BlockItemContext::decl() {
  return getRuleContext<SysYParserParser::DeclContext>(0);
}

SysYParserParser::StmtContext* SysYParserParser::BlockItemContext::stmt() {
  return getRuleContext<SysYParserParser::StmtContext>(0);
}


size_t SysYParserParser::BlockItemContext::getRuleIndex() const {
  return SysYParserParser::RuleBlockItem;
}

void SysYParserParser::BlockItemContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlockItem(this);
}

void SysYParserParser::BlockItemContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlockItem(this);
}


antlrcpp::Any SysYParserParser::BlockItemContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitBlockItem(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::BlockItemContext* SysYParserParser::blockItem() {
  BlockItemContext *_localctx = _tracker.createInstance<BlockItemContext>(_ctx, getState());
  enterRule(_localctx, 32, SysYParserParser::RuleBlockItem);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(219);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParserParser::T__0:
      case SysYParserParser::T__3:
      case SysYParserParser::T__4: {
        enterOuterAlt(_localctx, 1);
        setState(217);
        decl();
        break;
      }

      case SysYParserParser::T__2:
      case SysYParserParser::T__6:
      case SysYParserParser::T__10:
      case SysYParserParser::T__13:
      case SysYParserParser::T__15:
      case SysYParserParser::T__16:
      case SysYParserParser::T__17:
      case SysYParserParser::T__18:
      case SysYParserParser::T__19:
      case SysYParserParser::T__20:
      case SysYParserParser::T__21:
      case SysYParserParser::Identifier:
      case SysYParserParser::DecimalConstant:
      case SysYParserParser::OctalConstant:
      case SysYParserParser::HexadecimalConstant:
      case SysYParserParser::FloatingConstant: {
        enterOuterAlt(_localctx, 2);
        setState(218);
        stmt();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

SysYParserParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::StmtContext::getRuleIndex() const {
  return SysYParserParser::RuleStmt;
}

void SysYParserParser::StmtContext::copyFrom(StmtContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- StmtExpContext ------------------------------------------------------------------

SysYParserParser::ExpContext* SysYParserParser::StmtExpContext::exp() {
  return getRuleContext<SysYParserParser::ExpContext>(0);
}

SysYParserParser::StmtExpContext::StmtExpContext(StmtContext *ctx) { copyFrom(ctx); }

void SysYParserParser::StmtExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtExp(this);
}
void SysYParserParser::StmtExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtExp(this);
}

antlrcpp::Any SysYParserParser::StmtExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmtExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StmtBlockContext ------------------------------------------------------------------

SysYParserParser::BlockContext* SysYParserParser::StmtBlockContext::block() {
  return getRuleContext<SysYParserParser::BlockContext>(0);
}

SysYParserParser::StmtBlockContext::StmtBlockContext(StmtContext *ctx) { copyFrom(ctx); }

void SysYParserParser::StmtBlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtBlock(this);
}
void SysYParserParser::StmtBlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtBlock(this);
}

antlrcpp::Any SysYParserParser::StmtBlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmtBlock(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StmtBreakContext ------------------------------------------------------------------

SysYParserParser::StmtBreakContext::StmtBreakContext(StmtContext *ctx) { copyFrom(ctx); }

void SysYParserParser::StmtBreakContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtBreak(this);
}
void SysYParserParser::StmtBreakContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtBreak(this);
}

antlrcpp::Any SysYParserParser::StmtBreakContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmtBreak(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StmtWhileContext ------------------------------------------------------------------

SysYParserParser::CondContext* SysYParserParser::StmtWhileContext::cond() {
  return getRuleContext<SysYParserParser::CondContext>(0);
}

SysYParserParser::StmtContext* SysYParserParser::StmtWhileContext::stmt() {
  return getRuleContext<SysYParserParser::StmtContext>(0);
}

SysYParserParser::StmtWhileContext::StmtWhileContext(StmtContext *ctx) { copyFrom(ctx); }

void SysYParserParser::StmtWhileContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtWhile(this);
}
void SysYParserParser::StmtWhileContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtWhile(this);
}

antlrcpp::Any SysYParserParser::StmtWhileContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmtWhile(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StmtCondContext ------------------------------------------------------------------

SysYParserParser::CondContext* SysYParserParser::StmtCondContext::cond() {
  return getRuleContext<SysYParserParser::CondContext>(0);
}

std::vector<SysYParserParser::StmtContext *> SysYParserParser::StmtCondContext::stmt() {
  return getRuleContexts<SysYParserParser::StmtContext>();
}

SysYParserParser::StmtContext* SysYParserParser::StmtCondContext::stmt(size_t i) {
  return getRuleContext<SysYParserParser::StmtContext>(i);
}

SysYParserParser::StmtCondContext::StmtCondContext(StmtContext *ctx) { copyFrom(ctx); }

void SysYParserParser::StmtCondContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtCond(this);
}
void SysYParserParser::StmtCondContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtCond(this);
}

antlrcpp::Any SysYParserParser::StmtCondContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmtCond(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StmtContinueContext ------------------------------------------------------------------

SysYParserParser::StmtContinueContext::StmtContinueContext(StmtContext *ctx) { copyFrom(ctx); }

void SysYParserParser::StmtContinueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtContinue(this);
}
void SysYParserParser::StmtContinueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtContinue(this);
}

antlrcpp::Any SysYParserParser::StmtContinueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmtContinue(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StmtReturnContext ------------------------------------------------------------------

SysYParserParser::ExpContext* SysYParserParser::StmtReturnContext::exp() {
  return getRuleContext<SysYParserParser::ExpContext>(0);
}

SysYParserParser::StmtReturnContext::StmtReturnContext(StmtContext *ctx) { copyFrom(ctx); }

void SysYParserParser::StmtReturnContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtReturn(this);
}
void SysYParserParser::StmtReturnContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtReturn(this);
}

antlrcpp::Any SysYParserParser::StmtReturnContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmtReturn(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StmtAssignContext ------------------------------------------------------------------

SysYParserParser::LValContext* SysYParserParser::StmtAssignContext::lVal() {
  return getRuleContext<SysYParserParser::LValContext>(0);
}

SysYParserParser::ExpContext* SysYParserParser::StmtAssignContext::exp() {
  return getRuleContext<SysYParserParser::ExpContext>(0);
}

SysYParserParser::StmtAssignContext::StmtAssignContext(StmtContext *ctx) { copyFrom(ctx); }

void SysYParserParser::StmtAssignContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtAssign(this);
}
void SysYParserParser::StmtAssignContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtAssign(this);
}

antlrcpp::Any SysYParserParser::StmtAssignContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmtAssign(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::StmtContext* SysYParserParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 34, SysYParserParser::RuleStmt);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(255);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SysYParserParser::StmtAssignContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(221);
      lVal();
      setState(222);
      match(SysYParserParser::T__5);
      setState(223);
      exp();
      setState(224);
      match(SysYParserParser::T__2);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SysYParserParser::StmtExpContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(227);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParserParser::T__10)
        | (1ULL << SysYParserParser::T__19)
        | (1ULL << SysYParserParser::T__20)
        | (1ULL << SysYParserParser::T__21)
        | (1ULL << SysYParserParser::Identifier)
        | (1ULL << SysYParserParser::DecimalConstant)
        | (1ULL << SysYParserParser::OctalConstant)
        | (1ULL << SysYParserParser::HexadecimalConstant)
        | (1ULL << SysYParserParser::FloatingConstant))) != 0)) {
        setState(226);
        exp();
      }
      setState(229);
      match(SysYParserParser::T__2);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<SysYParserParser::StmtBlockContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(230);
      block();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<SysYParserParser::StmtCondContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(231);
      match(SysYParserParser::T__13);
      setState(232);
      match(SysYParserParser::T__10);
      setState(233);
      cond();
      setState(234);
      match(SysYParserParser::T__11);
      setState(235);
      stmt();
      setState(238);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx)) {
      case 1: {
        setState(236);
        match(SysYParserParser::T__14);
        setState(237);
        stmt();
        break;
      }

      default:
        break;
      }
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<SysYParserParser::StmtWhileContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(240);
      match(SysYParserParser::T__15);
      setState(241);
      match(SysYParserParser::T__10);
      setState(242);
      cond();
      setState(243);
      match(SysYParserParser::T__11);
      setState(244);
      stmt();
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<SysYParserParser::StmtBreakContext>(_localctx);
      enterOuterAlt(_localctx, 6);
      setState(246);
      match(SysYParserParser::T__16);
      setState(247);
      match(SysYParserParser::T__2);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<SysYParserParser::StmtContinueContext>(_localctx);
      enterOuterAlt(_localctx, 7);
      setState(248);
      match(SysYParserParser::T__17);
      setState(249);
      match(SysYParserParser::T__2);
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<SysYParserParser::StmtReturnContext>(_localctx);
      enterOuterAlt(_localctx, 8);
      setState(250);
      match(SysYParserParser::T__18);
      setState(252);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParserParser::T__10)
        | (1ULL << SysYParserParser::T__19)
        | (1ULL << SysYParserParser::T__20)
        | (1ULL << SysYParserParser::T__21)
        | (1ULL << SysYParserParser::Identifier)
        | (1ULL << SysYParserParser::DecimalConstant)
        | (1ULL << SysYParserParser::OctalConstant)
        | (1ULL << SysYParserParser::HexadecimalConstant)
        | (1ULL << SysYParserParser::FloatingConstant))) != 0)) {
        setState(251);
        exp();
      }
      setState(254);
      match(SysYParserParser::T__2);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpContext ------------------------------------------------------------------

SysYParserParser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::AddExpContext* SysYParserParser::ExpContext::addExp() {
  return getRuleContext<SysYParserParser::AddExpContext>(0);
}


size_t SysYParserParser::ExpContext::getRuleIndex() const {
  return SysYParserParser::RuleExp;
}

void SysYParserParser::ExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExp(this);
}

void SysYParserParser::ExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExp(this);
}


antlrcpp::Any SysYParserParser::ExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::ExpContext* SysYParserParser::exp() {
  ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, getState());
  enterRule(_localctx, 36, SysYParserParser::RuleExp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(257);
    addExp();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CondContext ------------------------------------------------------------------

SysYParserParser::CondContext::CondContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::LOrExpContext* SysYParserParser::CondContext::lOrExp() {
  return getRuleContext<SysYParserParser::LOrExpContext>(0);
}


size_t SysYParserParser::CondContext::getRuleIndex() const {
  return SysYParserParser::RuleCond;
}

void SysYParserParser::CondContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCond(this);
}

void SysYParserParser::CondContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCond(this);
}


antlrcpp::Any SysYParserParser::CondContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitCond(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::CondContext* SysYParserParser::cond() {
  CondContext *_localctx = _tracker.createInstance<CondContext>(_ctx, getState());
  enterRule(_localctx, 38, SysYParserParser::RuleCond);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(259);
    lOrExp();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LValContext ------------------------------------------------------------------

SysYParserParser::LValContext::LValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::LValContext::getRuleIndex() const {
  return SysYParserParser::RuleLVal;
}

void SysYParserParser::LValContext::copyFrom(LValContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- LValArrayContext ------------------------------------------------------------------

tree::TerminalNode* SysYParserParser::LValArrayContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

std::vector<SysYParserParser::ExpContext *> SysYParserParser::LValArrayContext::exp() {
  return getRuleContexts<SysYParserParser::ExpContext>();
}

SysYParserParser::ExpContext* SysYParserParser::LValArrayContext::exp(size_t i) {
  return getRuleContext<SysYParserParser::ExpContext>(i);
}

SysYParserParser::LValArrayContext::LValArrayContext(LValContext *ctx) { copyFrom(ctx); }

void SysYParserParser::LValArrayContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLValArray(this);
}
void SysYParserParser::LValArrayContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLValArray(this);
}

antlrcpp::Any SysYParserParser::LValArrayContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitLValArray(this);
  else
    return visitor->visitChildren(this);
}
//----------------- LValSingleContext ------------------------------------------------------------------

tree::TerminalNode* SysYParserParser::LValSingleContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

SysYParserParser::LValSingleContext::LValSingleContext(LValContext *ctx) { copyFrom(ctx); }

void SysYParserParser::LValSingleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLValSingle(this);
}
void SysYParserParser::LValSingleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLValSingle(this);
}

antlrcpp::Any SysYParserParser::LValSingleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitLValSingle(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::LValContext* SysYParserParser::lVal() {
  LValContext *_localctx = _tracker.createInstance<LValContext>(_ctx, getState());
  enterRule(_localctx, 40, SysYParserParser::RuleLVal);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(271);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SysYParserParser::LValSingleContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(261);
      match(SysYParserParser::Identifier);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SysYParserParser::LValArrayContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(262);
      match(SysYParserParser::Identifier);
      setState(267); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(263);
        match(SysYParserParser::T__8);
        setState(264);
        exp();
        setState(265);
        match(SysYParserParser::T__9);
        setState(269); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == SysYParserParser::T__8);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrimaryExpContext ------------------------------------------------------------------

SysYParserParser::PrimaryExpContext::PrimaryExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::PrimaryExpContext::getRuleIndex() const {
  return SysYParserParser::RulePrimaryExp;
}

void SysYParserParser::PrimaryExpContext::copyFrom(PrimaryExpContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- PrimaryExpParenContext ------------------------------------------------------------------

SysYParserParser::ExpContext* SysYParserParser::PrimaryExpParenContext::exp() {
  return getRuleContext<SysYParserParser::ExpContext>(0);
}

SysYParserParser::PrimaryExpParenContext::PrimaryExpParenContext(PrimaryExpContext *ctx) { copyFrom(ctx); }

void SysYParserParser::PrimaryExpParenContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryExpParen(this);
}
void SysYParserParser::PrimaryExpParenContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryExpParen(this);
}

antlrcpp::Any SysYParserParser::PrimaryExpParenContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitPrimaryExpParen(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryExpNumberContext ------------------------------------------------------------------

SysYParserParser::NumberContext* SysYParserParser::PrimaryExpNumberContext::number() {
  return getRuleContext<SysYParserParser::NumberContext>(0);
}

SysYParserParser::PrimaryExpNumberContext::PrimaryExpNumberContext(PrimaryExpContext *ctx) { copyFrom(ctx); }

void SysYParserParser::PrimaryExpNumberContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryExpNumber(this);
}
void SysYParserParser::PrimaryExpNumberContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryExpNumber(this);
}

antlrcpp::Any SysYParserParser::PrimaryExpNumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitPrimaryExpNumber(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryExpLValContext ------------------------------------------------------------------

SysYParserParser::LValContext* SysYParserParser::PrimaryExpLValContext::lVal() {
  return getRuleContext<SysYParserParser::LValContext>(0);
}

SysYParserParser::PrimaryExpLValContext::PrimaryExpLValContext(PrimaryExpContext *ctx) { copyFrom(ctx); }

void SysYParserParser::PrimaryExpLValContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryExpLVal(this);
}
void SysYParserParser::PrimaryExpLValContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryExpLVal(this);
}

antlrcpp::Any SysYParserParser::PrimaryExpLValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitPrimaryExpLVal(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::PrimaryExpContext* SysYParserParser::primaryExp() {
  PrimaryExpContext *_localctx = _tracker.createInstance<PrimaryExpContext>(_ctx, getState());
  enterRule(_localctx, 42, SysYParserParser::RulePrimaryExp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(279);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParserParser::T__10: {
        _localctx = _tracker.createInstance<SysYParserParser::PrimaryExpParenContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(273);
        match(SysYParserParser::T__10);
        setState(274);
        exp();
        setState(275);
        match(SysYParserParser::T__11);
        break;
      }

      case SysYParserParser::Identifier: {
        _localctx = _tracker.createInstance<SysYParserParser::PrimaryExpLValContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(277);
        lVal();
        break;
      }

      case SysYParserParser::DecimalConstant:
      case SysYParserParser::OctalConstant:
      case SysYParserParser::HexadecimalConstant:
      case SysYParserParser::FloatingConstant: {
        _localctx = _tracker.createInstance<SysYParserParser::PrimaryExpNumberContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(278);
        number();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberContext ------------------------------------------------------------------

SysYParserParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::IntConstContext* SysYParserParser::NumberContext::intConst() {
  return getRuleContext<SysYParserParser::IntConstContext>(0);
}

SysYParserParser::FloatConstContext* SysYParserParser::NumberContext::floatConst() {
  return getRuleContext<SysYParserParser::FloatConstContext>(0);
}


size_t SysYParserParser::NumberContext::getRuleIndex() const {
  return SysYParserParser::RuleNumber;
}

void SysYParserParser::NumberContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumber(this);
}

void SysYParserParser::NumberContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumber(this);
}


antlrcpp::Any SysYParserParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::NumberContext* SysYParserParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 44, SysYParserParser::RuleNumber);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(283);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParserParser::DecimalConstant:
      case SysYParserParser::OctalConstant:
      case SysYParserParser::HexadecimalConstant: {
        enterOuterAlt(_localctx, 1);
        setState(281);
        intConst();
        break;
      }

      case SysYParserParser::FloatingConstant: {
        enterOuterAlt(_localctx, 2);
        setState(282);
        floatConst();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryExpContext ------------------------------------------------------------------

SysYParserParser::UnaryExpContext::UnaryExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::UnaryExpContext::getRuleIndex() const {
  return SysYParserParser::RuleUnaryExp;
}

void SysYParserParser::UnaryExpContext::copyFrom(UnaryExpContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- UnaryExpFuncRContext ------------------------------------------------------------------

tree::TerminalNode* SysYParserParser::UnaryExpFuncRContext::Identifier() {
  return getToken(SysYParserParser::Identifier, 0);
}

SysYParserParser::FuncRParamsContext* SysYParserParser::UnaryExpFuncRContext::funcRParams() {
  return getRuleContext<SysYParserParser::FuncRParamsContext>(0);
}

SysYParserParser::UnaryExpFuncRContext::UnaryExpFuncRContext(UnaryExpContext *ctx) { copyFrom(ctx); }

void SysYParserParser::UnaryExpFuncRContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryExpFuncR(this);
}
void SysYParserParser::UnaryExpFuncRContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryExpFuncR(this);
}

antlrcpp::Any SysYParserParser::UnaryExpFuncRContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitUnaryExpFuncR(this);
  else
    return visitor->visitChildren(this);
}
//----------------- UnaryExpPrimaryExpContext ------------------------------------------------------------------

SysYParserParser::PrimaryExpContext* SysYParserParser::UnaryExpPrimaryExpContext::primaryExp() {
  return getRuleContext<SysYParserParser::PrimaryExpContext>(0);
}

SysYParserParser::UnaryExpPrimaryExpContext::UnaryExpPrimaryExpContext(UnaryExpContext *ctx) { copyFrom(ctx); }

void SysYParserParser::UnaryExpPrimaryExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryExpPrimaryExp(this);
}
void SysYParserParser::UnaryExpPrimaryExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryExpPrimaryExp(this);
}

antlrcpp::Any SysYParserParser::UnaryExpPrimaryExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitUnaryExpPrimaryExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- UnaryExpUnaryContext ------------------------------------------------------------------

SysYParserParser::UnaryOpContext* SysYParserParser::UnaryExpUnaryContext::unaryOp() {
  return getRuleContext<SysYParserParser::UnaryOpContext>(0);
}

SysYParserParser::UnaryExpContext* SysYParserParser::UnaryExpUnaryContext::unaryExp() {
  return getRuleContext<SysYParserParser::UnaryExpContext>(0);
}

SysYParserParser::UnaryExpUnaryContext::UnaryExpUnaryContext(UnaryExpContext *ctx) { copyFrom(ctx); }

void SysYParserParser::UnaryExpUnaryContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryExpUnary(this);
}
void SysYParserParser::UnaryExpUnaryContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryExpUnary(this);
}

antlrcpp::Any SysYParserParser::UnaryExpUnaryContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitUnaryExpUnary(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::UnaryExpContext* SysYParserParser::unaryExp() {
  UnaryExpContext *_localctx = _tracker.createInstance<UnaryExpContext>(_ctx, getState());
  enterRule(_localctx, 46, SysYParserParser::RuleUnaryExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(295);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SysYParserParser::UnaryExpPrimaryExpContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(285);
      primaryExp();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SysYParserParser::UnaryExpFuncRContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(286);
      match(SysYParserParser::Identifier);
      setState(287);
      match(SysYParserParser::T__10);
      setState(289);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParserParser::T__10)
        | (1ULL << SysYParserParser::T__19)
        | (1ULL << SysYParserParser::T__20)
        | (1ULL << SysYParserParser::T__21)
        | (1ULL << SysYParserParser::Identifier)
        | (1ULL << SysYParserParser::DecimalConstant)
        | (1ULL << SysYParserParser::OctalConstant)
        | (1ULL << SysYParserParser::HexadecimalConstant)
        | (1ULL << SysYParserParser::FloatingConstant)
        | (1ULL << SysYParserParser::StringLiteral))) != 0)) {
        setState(288);
        funcRParams();
      }
      setState(291);
      match(SysYParserParser::T__11);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<SysYParserParser::UnaryExpUnaryContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(292);
      unaryOp();
      setState(293);
      unaryExp();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryOpContext ------------------------------------------------------------------

SysYParserParser::UnaryOpContext::UnaryOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::UnaryOpContext::getRuleIndex() const {
  return SysYParserParser::RuleUnaryOp;
}

void SysYParserParser::UnaryOpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryOp(this);
}

void SysYParserParser::UnaryOpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryOp(this);
}


antlrcpp::Any SysYParserParser::UnaryOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitUnaryOp(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::UnaryOpContext* SysYParserParser::unaryOp() {
  UnaryOpContext *_localctx = _tracker.createInstance<UnaryOpContext>(_ctx, getState());
  enterRule(_localctx, 48, SysYParserParser::RuleUnaryOp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(297);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParserParser::T__19)
      | (1ULL << SysYParserParser::T__20)
      | (1ULL << SysYParserParser::T__21))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncRParamsContext ------------------------------------------------------------------

SysYParserParser::FuncRParamsContext::FuncRParamsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParserParser::FuncRParamContext *> SysYParserParser::FuncRParamsContext::funcRParam() {
  return getRuleContexts<SysYParserParser::FuncRParamContext>();
}

SysYParserParser::FuncRParamContext* SysYParserParser::FuncRParamsContext::funcRParam(size_t i) {
  return getRuleContext<SysYParserParser::FuncRParamContext>(i);
}


size_t SysYParserParser::FuncRParamsContext::getRuleIndex() const {
  return SysYParserParser::RuleFuncRParams;
}

void SysYParserParser::FuncRParamsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFuncRParams(this);
}

void SysYParserParser::FuncRParamsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFuncRParams(this);
}


antlrcpp::Any SysYParserParser::FuncRParamsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncRParams(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::FuncRParamsContext* SysYParserParser::funcRParams() {
  FuncRParamsContext *_localctx = _tracker.createInstance<FuncRParamsContext>(_ctx, getState());
  enterRule(_localctx, 50, SysYParserParser::RuleFuncRParams);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(299);
    funcRParam();
    setState(304);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParserParser::T__1) {
      setState(300);
      match(SysYParserParser::T__1);
      setState(301);
      funcRParam();
      setState(306);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncRParamContext ------------------------------------------------------------------

SysYParserParser::FuncRParamContext::FuncRParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::ExpContext* SysYParserParser::FuncRParamContext::exp() {
  return getRuleContext<SysYParserParser::ExpContext>(0);
}

tree::TerminalNode* SysYParserParser::FuncRParamContext::StringLiteral() {
  return getToken(SysYParserParser::StringLiteral, 0);
}


size_t SysYParserParser::FuncRParamContext::getRuleIndex() const {
  return SysYParserParser::RuleFuncRParam;
}

void SysYParserParser::FuncRParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFuncRParam(this);
}

void SysYParserParser::FuncRParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFuncRParam(this);
}


antlrcpp::Any SysYParserParser::FuncRParamContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncRParam(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::FuncRParamContext* SysYParserParser::funcRParam() {
  FuncRParamContext *_localctx = _tracker.createInstance<FuncRParamContext>(_ctx, getState());
  enterRule(_localctx, 52, SysYParserParser::RuleFuncRParam);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(309);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParserParser::T__10:
      case SysYParserParser::T__19:
      case SysYParserParser::T__20:
      case SysYParserParser::T__21:
      case SysYParserParser::Identifier:
      case SysYParserParser::DecimalConstant:
      case SysYParserParser::OctalConstant:
      case SysYParserParser::HexadecimalConstant:
      case SysYParserParser::FloatingConstant: {
        enterOuterAlt(_localctx, 1);
        setState(307);
        exp();
        break;
      }

      case SysYParserParser::StringLiteral: {
        enterOuterAlt(_localctx, 2);
        setState(308);
        match(SysYParserParser::StringLiteral);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MulExpContext ------------------------------------------------------------------

SysYParserParser::MulExpContext::MulExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParserParser::UnaryExpContext *> SysYParserParser::MulExpContext::unaryExp() {
  return getRuleContexts<SysYParserParser::UnaryExpContext>();
}

SysYParserParser::UnaryExpContext* SysYParserParser::MulExpContext::unaryExp(size_t i) {
  return getRuleContext<SysYParserParser::UnaryExpContext>(i);
}


size_t SysYParserParser::MulExpContext::getRuleIndex() const {
  return SysYParserParser::RuleMulExp;
}

void SysYParserParser::MulExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMulExp(this);
}

void SysYParserParser::MulExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMulExp(this);
}


antlrcpp::Any SysYParserParser::MulExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitMulExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::MulExpContext* SysYParserParser::mulExp() {
  MulExpContext *_localctx = _tracker.createInstance<MulExpContext>(_ctx, getState());
  enterRule(_localctx, 54, SysYParserParser::RuleMulExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(311);
    antlrcpp::downCast<MulExpContext *>(_localctx)->left = unaryExp();
    setState(316);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParserParser::T__22)
      | (1ULL << SysYParserParser::T__23)
      | (1ULL << SysYParserParser::T__24))) != 0)) {
      setState(312);
      antlrcpp::downCast<MulExpContext *>(_localctx)->_tset661 = _input->LT(1);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParserParser::T__22)
        | (1ULL << SysYParserParser::T__23)
        | (1ULL << SysYParserParser::T__24))) != 0))) {
        antlrcpp::downCast<MulExpContext *>(_localctx)->_tset661 = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      antlrcpp::downCast<MulExpContext *>(_localctx)->op.push_back(antlrcpp::downCast<MulExpContext *>(_localctx)->_tset661);
      setState(313);
      antlrcpp::downCast<MulExpContext *>(_localctx)->unaryExpContext = unaryExp();
      antlrcpp::downCast<MulExpContext *>(_localctx)->right.push_back(antlrcpp::downCast<MulExpContext *>(_localctx)->unaryExpContext);
      setState(318);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AddExpContext ------------------------------------------------------------------

SysYParserParser::AddExpContext::AddExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParserParser::MulExpContext *> SysYParserParser::AddExpContext::mulExp() {
  return getRuleContexts<SysYParserParser::MulExpContext>();
}

SysYParserParser::MulExpContext* SysYParserParser::AddExpContext::mulExp(size_t i) {
  return getRuleContext<SysYParserParser::MulExpContext>(i);
}


size_t SysYParserParser::AddExpContext::getRuleIndex() const {
  return SysYParserParser::RuleAddExp;
}

void SysYParserParser::AddExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAddExp(this);
}

void SysYParserParser::AddExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAddExp(this);
}


antlrcpp::Any SysYParserParser::AddExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitAddExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::AddExpContext* SysYParserParser::addExp() {
  AddExpContext *_localctx = _tracker.createInstance<AddExpContext>(_ctx, getState());
  enterRule(_localctx, 56, SysYParserParser::RuleAddExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(319);
    antlrcpp::downCast<AddExpContext *>(_localctx)->left = mulExp();
    setState(324);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParserParser::T__19

    || _la == SysYParserParser::T__20) {
      setState(320);
      antlrcpp::downCast<AddExpContext *>(_localctx)->_tset694 = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == SysYParserParser::T__19

      || _la == SysYParserParser::T__20)) {
        antlrcpp::downCast<AddExpContext *>(_localctx)->_tset694 = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      antlrcpp::downCast<AddExpContext *>(_localctx)->op.push_back(antlrcpp::downCast<AddExpContext *>(_localctx)->_tset694);
      setState(321);
      antlrcpp::downCast<AddExpContext *>(_localctx)->mulExpContext = mulExp();
      antlrcpp::downCast<AddExpContext *>(_localctx)->right.push_back(antlrcpp::downCast<AddExpContext *>(_localctx)->mulExpContext);
      setState(326);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RelExpContext ------------------------------------------------------------------

SysYParserParser::RelExpContext::RelExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParserParser::AddExpContext *> SysYParserParser::RelExpContext::addExp() {
  return getRuleContexts<SysYParserParser::AddExpContext>();
}

SysYParserParser::AddExpContext* SysYParserParser::RelExpContext::addExp(size_t i) {
  return getRuleContext<SysYParserParser::AddExpContext>(i);
}


size_t SysYParserParser::RelExpContext::getRuleIndex() const {
  return SysYParserParser::RuleRelExp;
}

void SysYParserParser::RelExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRelExp(this);
}

void SysYParserParser::RelExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRelExp(this);
}


antlrcpp::Any SysYParserParser::RelExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitRelExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::RelExpContext* SysYParserParser::relExp() {
  RelExpContext *_localctx = _tracker.createInstance<RelExpContext>(_ctx, getState());
  enterRule(_localctx, 58, SysYParserParser::RuleRelExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(327);
    antlrcpp::downCast<RelExpContext *>(_localctx)->left = addExp();
    setState(332);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParserParser::T__25)
      | (1ULL << SysYParserParser::T__26)
      | (1ULL << SysYParserParser::T__27)
      | (1ULL << SysYParserParser::T__28))) != 0)) {
      setState(328);
      antlrcpp::downCast<RelExpContext *>(_localctx)->_tset722 = _input->LT(1);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParserParser::T__25)
        | (1ULL << SysYParserParser::T__26)
        | (1ULL << SysYParserParser::T__27)
        | (1ULL << SysYParserParser::T__28))) != 0))) {
        antlrcpp::downCast<RelExpContext *>(_localctx)->_tset722 = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      antlrcpp::downCast<RelExpContext *>(_localctx)->op.push_back(antlrcpp::downCast<RelExpContext *>(_localctx)->_tset722);
      setState(329);
      antlrcpp::downCast<RelExpContext *>(_localctx)->addExpContext = addExp();
      antlrcpp::downCast<RelExpContext *>(_localctx)->right.push_back(antlrcpp::downCast<RelExpContext *>(_localctx)->addExpContext);
      setState(334);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EqExpContext ------------------------------------------------------------------

SysYParserParser::EqExpContext::EqExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParserParser::RelExpContext *> SysYParserParser::EqExpContext::relExp() {
  return getRuleContexts<SysYParserParser::RelExpContext>();
}

SysYParserParser::RelExpContext* SysYParserParser::EqExpContext::relExp(size_t i) {
  return getRuleContext<SysYParserParser::RelExpContext>(i);
}


size_t SysYParserParser::EqExpContext::getRuleIndex() const {
  return SysYParserParser::RuleEqExp;
}

void SysYParserParser::EqExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEqExp(this);
}

void SysYParserParser::EqExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEqExp(this);
}


antlrcpp::Any SysYParserParser::EqExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitEqExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::EqExpContext* SysYParserParser::eqExp() {
  EqExpContext *_localctx = _tracker.createInstance<EqExpContext>(_ctx, getState());
  enterRule(_localctx, 60, SysYParserParser::RuleEqExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(335);
    antlrcpp::downCast<EqExpContext *>(_localctx)->left = relExp();
    setState(340);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParserParser::T__29

    || _la == SysYParserParser::T__30) {
      setState(336);
      antlrcpp::downCast<EqExpContext *>(_localctx)->_tset757 = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == SysYParserParser::T__29

      || _la == SysYParserParser::T__30)) {
        antlrcpp::downCast<EqExpContext *>(_localctx)->_tset757 = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      antlrcpp::downCast<EqExpContext *>(_localctx)->op.push_back(antlrcpp::downCast<EqExpContext *>(_localctx)->_tset757);
      setState(337);
      antlrcpp::downCast<EqExpContext *>(_localctx)->relExpContext = relExp();
      antlrcpp::downCast<EqExpContext *>(_localctx)->right.push_back(antlrcpp::downCast<EqExpContext *>(_localctx)->relExpContext);
      setState(342);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LAndExpContext ------------------------------------------------------------------

SysYParserParser::LAndExpContext::LAndExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParserParser::EqExpContext *> SysYParserParser::LAndExpContext::eqExp() {
  return getRuleContexts<SysYParserParser::EqExpContext>();
}

SysYParserParser::EqExpContext* SysYParserParser::LAndExpContext::eqExp(size_t i) {
  return getRuleContext<SysYParserParser::EqExpContext>(i);
}


size_t SysYParserParser::LAndExpContext::getRuleIndex() const {
  return SysYParserParser::RuleLAndExp;
}

void SysYParserParser::LAndExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLAndExp(this);
}

void SysYParserParser::LAndExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLAndExp(this);
}


antlrcpp::Any SysYParserParser::LAndExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitLAndExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::LAndExpContext* SysYParserParser::lAndExp() {
  LAndExpContext *_localctx = _tracker.createInstance<LAndExpContext>(_ctx, getState());
  enterRule(_localctx, 62, SysYParserParser::RuleLAndExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(343);
    antlrcpp::downCast<LAndExpContext *>(_localctx)->left = eqExp();
    setState(348);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParserParser::T__31) {
      setState(344);
      antlrcpp::downCast<LAndExpContext *>(_localctx)->s32 = match(SysYParserParser::T__31);
      antlrcpp::downCast<LAndExpContext *>(_localctx)->op.push_back(antlrcpp::downCast<LAndExpContext *>(_localctx)->s32);
      setState(345);
      antlrcpp::downCast<LAndExpContext *>(_localctx)->eqExpContext = eqExp();
      antlrcpp::downCast<LAndExpContext *>(_localctx)->right.push_back(antlrcpp::downCast<LAndExpContext *>(_localctx)->eqExpContext);
      setState(350);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LOrExpContext ------------------------------------------------------------------

SysYParserParser::LOrExpContext::LOrExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParserParser::LAndExpContext *> SysYParserParser::LOrExpContext::lAndExp() {
  return getRuleContexts<SysYParserParser::LAndExpContext>();
}

SysYParserParser::LAndExpContext* SysYParserParser::LOrExpContext::lAndExp(size_t i) {
  return getRuleContext<SysYParserParser::LAndExpContext>(i);
}


size_t SysYParserParser::LOrExpContext::getRuleIndex() const {
  return SysYParserParser::RuleLOrExp;
}

void SysYParserParser::LOrExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLOrExp(this);
}

void SysYParserParser::LOrExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLOrExp(this);
}


antlrcpp::Any SysYParserParser::LOrExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitLOrExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::LOrExpContext* SysYParserParser::lOrExp() {
  LOrExpContext *_localctx = _tracker.createInstance<LOrExpContext>(_ctx, getState());
  enterRule(_localctx, 64, SysYParserParser::RuleLOrExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(351);
    antlrcpp::downCast<LOrExpContext *>(_localctx)->left = lAndExp();
    setState(356);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParserParser::T__32) {
      setState(352);
      antlrcpp::downCast<LOrExpContext *>(_localctx)->s33 = match(SysYParserParser::T__32);
      antlrcpp::downCast<LOrExpContext *>(_localctx)->op.push_back(antlrcpp::downCast<LOrExpContext *>(_localctx)->s33);
      setState(353);
      antlrcpp::downCast<LOrExpContext *>(_localctx)->lAndExpContext = lAndExp();
      antlrcpp::downCast<LOrExpContext *>(_localctx)->right.push_back(antlrcpp::downCast<LOrExpContext *>(_localctx)->lAndExpContext);
      setState(358);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstExpContext ------------------------------------------------------------------

SysYParserParser::ConstExpContext::ConstExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParserParser::AddExpContext* SysYParserParser::ConstExpContext::addExp() {
  return getRuleContext<SysYParserParser::AddExpContext>(0);
}


size_t SysYParserParser::ConstExpContext::getRuleIndex() const {
  return SysYParserParser::RuleConstExp;
}

void SysYParserParser::ConstExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstExp(this);
}

void SysYParserParser::ConstExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstExp(this);
}


antlrcpp::Any SysYParserParser::ConstExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::ConstExpContext* SysYParserParser::constExp() {
  ConstExpContext *_localctx = _tracker.createInstance<ConstExpContext>(_ctx, getState());
  enterRule(_localctx, 66, SysYParserParser::RuleConstExp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(359);
    addExp();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IntConstContext ------------------------------------------------------------------

SysYParserParser::IntConstContext::IntConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SysYParserParser::IntConstContext::getRuleIndex() const {
  return SysYParserParser::RuleIntConst;
}

void SysYParserParser::IntConstContext::copyFrom(IntConstContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- IntHexConstContext ------------------------------------------------------------------

tree::TerminalNode* SysYParserParser::IntHexConstContext::HexadecimalConstant() {
  return getToken(SysYParserParser::HexadecimalConstant, 0);
}

SysYParserParser::IntHexConstContext::IntHexConstContext(IntConstContext *ctx) { copyFrom(ctx); }

void SysYParserParser::IntHexConstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIntHexConst(this);
}
void SysYParserParser::IntHexConstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIntHexConst(this);
}

antlrcpp::Any SysYParserParser::IntHexConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitIntHexConst(this);
  else
    return visitor->visitChildren(this);
}
//----------------- IntDecConstContext ------------------------------------------------------------------

tree::TerminalNode* SysYParserParser::IntDecConstContext::DecimalConstant() {
  return getToken(SysYParserParser::DecimalConstant, 0);
}

SysYParserParser::IntDecConstContext::IntDecConstContext(IntConstContext *ctx) { copyFrom(ctx); }

void SysYParserParser::IntDecConstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIntDecConst(this);
}
void SysYParserParser::IntDecConstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIntDecConst(this);
}

antlrcpp::Any SysYParserParser::IntDecConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitIntDecConst(this);
  else
    return visitor->visitChildren(this);
}
//----------------- IntOctConstContext ------------------------------------------------------------------

tree::TerminalNode* SysYParserParser::IntOctConstContext::OctalConstant() {
  return getToken(SysYParserParser::OctalConstant, 0);
}

SysYParserParser::IntOctConstContext::IntOctConstContext(IntConstContext *ctx) { copyFrom(ctx); }

void SysYParserParser::IntOctConstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIntOctConst(this);
}
void SysYParserParser::IntOctConstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIntOctConst(this);
}

antlrcpp::Any SysYParserParser::IntOctConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitIntOctConst(this);
  else
    return visitor->visitChildren(this);
}
SysYParserParser::IntConstContext* SysYParserParser::intConst() {
  IntConstContext *_localctx = _tracker.createInstance<IntConstContext>(_ctx, getState());
  enterRule(_localctx, 68, SysYParserParser::RuleIntConst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(364);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParserParser::DecimalConstant: {
        _localctx = _tracker.createInstance<SysYParserParser::IntDecConstContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(361);
        match(SysYParserParser::DecimalConstant);
        break;
      }

      case SysYParserParser::OctalConstant: {
        _localctx = _tracker.createInstance<SysYParserParser::IntOctConstContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(362);
        match(SysYParserParser::OctalConstant);
        break;
      }

      case SysYParserParser::HexadecimalConstant: {
        _localctx = _tracker.createInstance<SysYParserParser::IntHexConstContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(363);
        match(SysYParserParser::HexadecimalConstant);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FloatConstContext ------------------------------------------------------------------

SysYParserParser::FloatConstContext::FloatConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParserParser::FloatConstContext::FloatingConstant() {
  return getToken(SysYParserParser::FloatingConstant, 0);
}


size_t SysYParserParser::FloatConstContext::getRuleIndex() const {
  return SysYParserParser::RuleFloatConst;
}

void SysYParserParser::FloatConstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFloatConst(this);
}

void SysYParserParser::FloatConstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SysYParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFloatConst(this);
}


antlrcpp::Any SysYParserParser::FloatConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFloatConst(this);
  else
    return visitor->visitChildren(this);
}

SysYParserParser::FloatConstContext* SysYParserParser::floatConst() {
  FloatConstContext *_localctx = _tracker.createInstance<FloatConstContext>(_ctx, getState());
  enterRule(_localctx, 70, SysYParserParser::RuleFloatConst);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(366);
    match(SysYParserParser::FloatingConstant);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

// Static vars and initialization.
std::vector<dfa::DFA> SysYParserParser::_decisionToDFA;
atn::PredictionContextCache SysYParserParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN SysYParserParser::_atn;
std::vector<uint16_t> SysYParserParser::_serializedATN;

std::vector<std::string> SysYParserParser::_ruleNames = {
  "program", "compUnit", "decl", "constDecl", "bType", "constDef", "constInitVal", 
  "varDecl", "varDef", "initLVal", "initVal", "funcDef", "funcType", "funcFParams", 
  "funcFParam", "block", "blockItem", "stmt", "exp", "cond", "lVal", "primaryExp", 
  "number", "unaryExp", "unaryOp", "funcRParams", "funcRParam", "mulExp", 
  "addExp", "relExp", "eqExp", "lAndExp", "lOrExp", "constExp", "intConst", 
  "floatConst"
};

std::vector<std::string> SysYParserParser::_literalNames = {
  "", "'const'", "','", "';'", "'int'", "'float'", "'='", "'{'", "'}'", 
  "'['", "']'", "'('", "')'", "'void'", "'if'", "'else'", "'while'", "'break'", 
  "'continue'", "'return'", "'+'", "'-'", "'!'", "'*'", "'/'", "'%'", "'<'", 
  "'>'", "'<='", "'>='", "'=='", "'!='", "'&&'", "'||'"
};

std::vector<std::string> SysYParserParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "Identifier", 
  "DecimalConstant", "OctalConstant", "HexadecimalConstant", "FloatingConstant", 
  "StringLiteral", "Whitespace", "Newline", "BlockComment", "LineComment"
};

dfa::Vocabulary SysYParserParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> SysYParserParser::_tokenNames;

SysYParserParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  static const uint16_t serializedATNSegment0[] = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
       0x3, 0x2d, 0x173, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
       0x9, 0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 
       0x7, 0x4, 0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 
       0x4, 0xb, 0x9, 0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 
       0xe, 0x9, 0xe, 0x4, 0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 
       0x9, 0x11, 0x4, 0x12, 0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 
       0x9, 0x14, 0x4, 0x15, 0x9, 0x15, 0x4, 0x16, 0x9, 0x16, 0x4, 0x17, 
       0x9, 0x17, 0x4, 0x18, 0x9, 0x18, 0x4, 0x19, 0x9, 0x19, 0x4, 0x1a, 
       0x9, 0x1a, 0x4, 0x1b, 0x9, 0x1b, 0x4, 0x1c, 0x9, 0x1c, 0x4, 0x1d, 
       0x9, 0x1d, 0x4, 0x1e, 0x9, 0x1e, 0x4, 0x1f, 0x9, 0x1f, 0x4, 0x20, 
       0x9, 0x20, 0x4, 0x21, 0x9, 0x21, 0x4, 0x22, 0x9, 0x22, 0x4, 0x23, 
       0x9, 0x23, 0x4, 0x24, 0x9, 0x24, 0x4, 0x25, 0x9, 0x25, 0x3, 0x2, 
       0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x4f, 0xa, 0x3, 0xc, 0x3, 
       0xe, 0x3, 0x52, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x4, 
       0x5, 0x4, 0x58, 0xa, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 
       0x3, 0x5, 0x7, 0x5, 0x5f, 0xa, 0x5, 0xc, 0x5, 0xe, 0x5, 0x62, 0xb, 
       0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 0x3, 0x7, 
       0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x5, 0x7, 0x6f, 
       0xa, 0x7, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x7, 
       0x8, 0x76, 0xa, 0x8, 0xc, 0x8, 0xe, 0x8, 0x79, 0xb, 0x8, 0x5, 0x8, 
       0x7b, 0xa, 0x8, 0x3, 0x8, 0x5, 0x8, 0x7e, 0xa, 0x8, 0x3, 0x9, 0x3, 
       0x9, 0x3, 0x9, 0x3, 0x9, 0x7, 0x9, 0x84, 0xa, 0x9, 0xc, 0x9, 0xe, 
       0x9, 0x87, 0xb, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 
       0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 
       0x5, 0xa, 0x94, 0xa, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
       0x3, 0xb, 0x6, 0xb, 0x9b, 0xa, 0xb, 0xd, 0xb, 0xe, 0xb, 0x9c, 0x3, 
       0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x7, 0xc, 0xa4, 0xa, 
       0xc, 0xc, 0xc, 0xe, 0xc, 0xa7, 0xb, 0xc, 0x5, 0xc, 0xa9, 0xa, 0xc, 
       0x3, 0xc, 0x5, 0xc, 0xac, 0xa, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 
       0x3, 0xd, 0x5, 0xd, 0xb2, 0xa, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 
       0x3, 0xe, 0x3, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x7, 0xf, 0xbc, 
       0xa, 0xf, 0xc, 0xf, 0xe, 0xf, 0xbf, 0xb, 0xf, 0x3, 0x10, 0x3, 0x10, 
       0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 
       0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x7, 0x10, 0xcc, 0xa, 0x10, 0xc, 
       0x10, 0xe, 0x10, 0xcf, 0xb, 0x10, 0x5, 0x10, 0xd1, 0xa, 0x10, 0x3, 
       0x11, 0x3, 0x11, 0x7, 0x11, 0xd5, 0xa, 0x11, 0xc, 0x11, 0xe, 0x11, 
       0xd8, 0xb, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x12, 0x3, 0x12, 0x5, 
       0x12, 0xde, 0xa, 0x12, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 
       0x3, 0x13, 0x3, 0x13, 0x5, 0x13, 0xe6, 0xa, 0x13, 0x3, 0x13, 0x3, 
       0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 
       0x13, 0x3, 0x13, 0x5, 0x13, 0xf1, 0xa, 0x13, 0x3, 0x13, 0x3, 0x13, 
       0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 
       0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x5, 0x13, 0xff, 0xa, 
       0x13, 0x3, 0x13, 0x5, 0x13, 0x102, 0xa, 0x13, 0x3, 0x14, 0x3, 0x14, 
       0x3, 0x15, 0x3, 0x15, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 
       0x3, 0x16, 0x3, 0x16, 0x6, 0x16, 0x10e, 0xa, 0x16, 0xd, 0x16, 0xe, 
       0x16, 0x10f, 0x5, 0x16, 0x112, 0xa, 0x16, 0x3, 0x17, 0x3, 0x17, 0x3, 
       0x17, 0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x5, 0x17, 0x11a, 0xa, 0x17, 
       0x3, 0x18, 0x3, 0x18, 0x5, 0x18, 0x11e, 0xa, 0x18, 0x3, 0x19, 0x3, 
       0x19, 0x3, 0x19, 0x3, 0x19, 0x5, 0x19, 0x124, 0xa, 0x19, 0x3, 0x19, 
       0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x5, 0x19, 0x12a, 0xa, 0x19, 0x3, 
       0x1a, 0x3, 0x1a, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x7, 0x1b, 0x131, 
       0xa, 0x1b, 0xc, 0x1b, 0xe, 0x1b, 0x134, 0xb, 0x1b, 0x3, 0x1c, 0x3, 
       0x1c, 0x5, 0x1c, 0x138, 0xa, 0x1c, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 
       0x7, 0x1d, 0x13d, 0xa, 0x1d, 0xc, 0x1d, 0xe, 0x1d, 0x140, 0xb, 0x1d, 
       0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 0x7, 0x1e, 0x145, 0xa, 0x1e, 0xc, 
       0x1e, 0xe, 0x1e, 0x148, 0xb, 0x1e, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 
       0x7, 0x1f, 0x14d, 0xa, 0x1f, 0xc, 0x1f, 0xe, 0x1f, 0x150, 0xb, 0x1f, 
       0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x7, 0x20, 0x155, 0xa, 0x20, 0xc, 
       0x20, 0xe, 0x20, 0x158, 0xb, 0x20, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 
       0x7, 0x21, 0x15d, 0xa, 0x21, 0xc, 0x21, 0xe, 0x21, 0x160, 0xb, 0x21, 
       0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x7, 0x22, 0x165, 0xa, 0x22, 0xc, 
       0x22, 0xe, 0x22, 0x168, 0xb, 0x22, 0x3, 0x23, 0x3, 0x23, 0x3, 0x24, 
       0x3, 0x24, 0x3, 0x24, 0x5, 0x24, 0x16f, 0xa, 0x24, 0x3, 0x25, 0x3, 
       0x25, 0x3, 0x25, 0x2, 0x2, 0x26, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 
       0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 
       0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 
       0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x48, 0x2, 0x9, 0x3, 0x2, 0x6, 
       0x7, 0x4, 0x2, 0x6, 0x7, 0xf, 0xf, 0x3, 0x2, 0x16, 0x18, 0x3, 0x2, 
       0x19, 0x1b, 0x3, 0x2, 0x16, 0x17, 0x3, 0x2, 0x1c, 0x1f, 0x3, 0x2, 
       0x20, 0x21, 0x2, 0x180, 0x2, 0x4a, 0x3, 0x2, 0x2, 0x2, 0x4, 0x50, 
       0x3, 0x2, 0x2, 0x2, 0x6, 0x57, 0x3, 0x2, 0x2, 0x2, 0x8, 0x59, 0x3, 
       0x2, 0x2, 0x2, 0xa, 0x65, 0x3, 0x2, 0x2, 0x2, 0xc, 0x6e, 0x3, 0x2, 
       0x2, 0x2, 0xe, 0x7d, 0x3, 0x2, 0x2, 0x2, 0x10, 0x7f, 0x3, 0x2, 0x2, 
       0x2, 0x12, 0x93, 0x3, 0x2, 0x2, 0x2, 0x14, 0x95, 0x3, 0x2, 0x2, 0x2, 
       0x16, 0xab, 0x3, 0x2, 0x2, 0x2, 0x18, 0xad, 0x3, 0x2, 0x2, 0x2, 0x1a, 
       0xb6, 0x3, 0x2, 0x2, 0x2, 0x1c, 0xb8, 0x3, 0x2, 0x2, 0x2, 0x1e, 0xd0, 
       0x3, 0x2, 0x2, 0x2, 0x20, 0xd2, 0x3, 0x2, 0x2, 0x2, 0x22, 0xdd, 0x3, 
       0x2, 0x2, 0x2, 0x24, 0x101, 0x3, 0x2, 0x2, 0x2, 0x26, 0x103, 0x3, 
       0x2, 0x2, 0x2, 0x28, 0x105, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x111, 0x3, 
       0x2, 0x2, 0x2, 0x2c, 0x119, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x11d, 0x3, 
       0x2, 0x2, 0x2, 0x30, 0x129, 0x3, 0x2, 0x2, 0x2, 0x32, 0x12b, 0x3, 
       0x2, 0x2, 0x2, 0x34, 0x12d, 0x3, 0x2, 0x2, 0x2, 0x36, 0x137, 0x3, 
       0x2, 0x2, 0x2, 0x38, 0x139, 0x3, 0x2, 0x2, 0x2, 0x3a, 0x141, 0x3, 
       0x2, 0x2, 0x2, 0x3c, 0x149, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x151, 0x3, 
       0x2, 0x2, 0x2, 0x40, 0x159, 0x3, 0x2, 0x2, 0x2, 0x42, 0x161, 0x3, 
       0x2, 0x2, 0x2, 0x44, 0x169, 0x3, 0x2, 0x2, 0x2, 0x46, 0x16e, 0x3, 
       0x2, 0x2, 0x2, 0x48, 0x170, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x4b, 0x5, 
       0x4, 0x3, 0x2, 0x4b, 0x3, 0x3, 0x2, 0x2, 0x2, 0x4c, 0x4f, 0x5, 0x6, 
       0x4, 0x2, 0x4d, 0x4f, 0x5, 0x18, 0xd, 0x2, 0x4e, 0x4c, 0x3, 0x2, 
       0x2, 0x2, 0x4e, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x4f, 0x52, 0x3, 0x2, 0x2, 
       0x2, 0x50, 0x4e, 0x3, 0x2, 0x2, 0x2, 0x50, 0x51, 0x3, 0x2, 0x2, 0x2, 
       0x51, 0x53, 0x3, 0x2, 0x2, 0x2, 0x52, 0x50, 0x3, 0x2, 0x2, 0x2, 0x53, 
       0x54, 0x7, 0x2, 0x2, 0x3, 0x54, 0x5, 0x3, 0x2, 0x2, 0x2, 0x55, 0x58, 
       0x5, 0x8, 0x5, 0x2, 0x56, 0x58, 0x5, 0x10, 0x9, 0x2, 0x57, 0x55, 
       0x3, 0x2, 0x2, 0x2, 0x57, 0x56, 0x3, 0x2, 0x2, 0x2, 0x58, 0x7, 0x3, 
       0x2, 0x2, 0x2, 0x59, 0x5a, 0x7, 0x3, 0x2, 0x2, 0x5a, 0x5b, 0x5, 0xa, 
       0x6, 0x2, 0x5b, 0x60, 0x5, 0xc, 0x7, 0x2, 0x5c, 0x5d, 0x7, 0x4, 0x2, 
       0x2, 0x5d, 0x5f, 0x5, 0xc, 0x7, 0x2, 0x5e, 0x5c, 0x3, 0x2, 0x2, 0x2, 
       0x5f, 0x62, 0x3, 0x2, 0x2, 0x2, 0x60, 0x5e, 0x3, 0x2, 0x2, 0x2, 0x60, 
       0x61, 0x3, 0x2, 0x2, 0x2, 0x61, 0x63, 0x3, 0x2, 0x2, 0x2, 0x62, 0x60, 
       0x3, 0x2, 0x2, 0x2, 0x63, 0x64, 0x7, 0x5, 0x2, 0x2, 0x64, 0x9, 0x3, 
       0x2, 0x2, 0x2, 0x65, 0x66, 0x9, 0x2, 0x2, 0x2, 0x66, 0xb, 0x3, 0x2, 
       0x2, 0x2, 0x67, 0x68, 0x7, 0x24, 0x2, 0x2, 0x68, 0x69, 0x7, 0x8, 
       0x2, 0x2, 0x69, 0x6f, 0x5, 0xe, 0x8, 0x2, 0x6a, 0x6b, 0x5, 0x14, 
       0xb, 0x2, 0x6b, 0x6c, 0x7, 0x8, 0x2, 0x2, 0x6c, 0x6d, 0x5, 0xe, 0x8, 
       0x2, 0x6d, 0x6f, 0x3, 0x2, 0x2, 0x2, 0x6e, 0x67, 0x3, 0x2, 0x2, 0x2, 
       0x6e, 0x6a, 0x3, 0x2, 0x2, 0x2, 0x6f, 0xd, 0x3, 0x2, 0x2, 0x2, 0x70, 
       0x7e, 0x5, 0x44, 0x23, 0x2, 0x71, 0x7a, 0x7, 0x9, 0x2, 0x2, 0x72, 
       0x77, 0x5, 0xe, 0x8, 0x2, 0x73, 0x74, 0x7, 0x4, 0x2, 0x2, 0x74, 0x76, 
       0x5, 0xe, 0x8, 0x2, 0x75, 0x73, 0x3, 0x2, 0x2, 0x2, 0x76, 0x79, 0x3, 
       0x2, 0x2, 0x2, 0x77, 0x75, 0x3, 0x2, 0x2, 0x2, 0x77, 0x78, 0x3, 0x2, 
       0x2, 0x2, 0x78, 0x7b, 0x3, 0x2, 0x2, 0x2, 0x79, 0x77, 0x3, 0x2, 0x2, 
       0x2, 0x7a, 0x72, 0x3, 0x2, 0x2, 0x2, 0x7a, 0x7b, 0x3, 0x2, 0x2, 0x2, 
       0x7b, 0x7c, 0x3, 0x2, 0x2, 0x2, 0x7c, 0x7e, 0x7, 0xa, 0x2, 0x2, 0x7d, 
       0x70, 0x3, 0x2, 0x2, 0x2, 0x7d, 0x71, 0x3, 0x2, 0x2, 0x2, 0x7e, 0xf, 
       0x3, 0x2, 0x2, 0x2, 0x7f, 0x80, 0x5, 0xa, 0x6, 0x2, 0x80, 0x85, 0x5, 
       0x12, 0xa, 0x2, 0x81, 0x82, 0x7, 0x4, 0x2, 0x2, 0x82, 0x84, 0x5, 
       0x12, 0xa, 0x2, 0x83, 0x81, 0x3, 0x2, 0x2, 0x2, 0x84, 0x87, 0x3, 
       0x2, 0x2, 0x2, 0x85, 0x83, 0x3, 0x2, 0x2, 0x2, 0x85, 0x86, 0x3, 0x2, 
       0x2, 0x2, 0x86, 0x88, 0x3, 0x2, 0x2, 0x2, 0x87, 0x85, 0x3, 0x2, 0x2, 
       0x2, 0x88, 0x89, 0x7, 0x5, 0x2, 0x2, 0x89, 0x11, 0x3, 0x2, 0x2, 0x2, 
       0x8a, 0x94, 0x7, 0x24, 0x2, 0x2, 0x8b, 0x94, 0x5, 0x14, 0xb, 0x2, 
       0x8c, 0x8d, 0x7, 0x24, 0x2, 0x2, 0x8d, 0x8e, 0x7, 0x8, 0x2, 0x2, 
       0x8e, 0x94, 0x5, 0x16, 0xc, 0x2, 0x8f, 0x90, 0x5, 0x14, 0xb, 0x2, 
       0x90, 0x91, 0x7, 0x8, 0x2, 0x2, 0x91, 0x92, 0x5, 0x16, 0xc, 0x2, 
       0x92, 0x94, 0x3, 0x2, 0x2, 0x2, 0x93, 0x8a, 0x3, 0x2, 0x2, 0x2, 0x93, 
       0x8b, 0x3, 0x2, 0x2, 0x2, 0x93, 0x8c, 0x3, 0x2, 0x2, 0x2, 0x93, 0x8f, 
       0x3, 0x2, 0x2, 0x2, 0x94, 0x13, 0x3, 0x2, 0x2, 0x2, 0x95, 0x9a, 0x7, 
       0x24, 0x2, 0x2, 0x96, 0x97, 0x7, 0xb, 0x2, 0x2, 0x97, 0x98, 0x5, 
       0x44, 0x23, 0x2, 0x98, 0x99, 0x7, 0xc, 0x2, 0x2, 0x99, 0x9b, 0x3, 
       0x2, 0x2, 0x2, 0x9a, 0x96, 0x3, 0x2, 0x2, 0x2, 0x9b, 0x9c, 0x3, 0x2, 
       0x2, 0x2, 0x9c, 0x9a, 0x3, 0x2, 0x2, 0x2, 0x9c, 0x9d, 0x3, 0x2, 0x2, 
       0x2, 0x9d, 0x15, 0x3, 0x2, 0x2, 0x2, 0x9e, 0xac, 0x5, 0x26, 0x14, 
       0x2, 0x9f, 0xa8, 0x7, 0x9, 0x2, 0x2, 0xa0, 0xa5, 0x5, 0x16, 0xc, 
       0x2, 0xa1, 0xa2, 0x7, 0x4, 0x2, 0x2, 0xa2, 0xa4, 0x5, 0x16, 0xc, 
       0x2, 0xa3, 0xa1, 0x3, 0x2, 0x2, 0x2, 0xa4, 0xa7, 0x3, 0x2, 0x2, 0x2, 
       0xa5, 0xa3, 0x3, 0x2, 0x2, 0x2, 0xa5, 0xa6, 0x3, 0x2, 0x2, 0x2, 0xa6, 
       0xa9, 0x3, 0x2, 0x2, 0x2, 0xa7, 0xa5, 0x3, 0x2, 0x2, 0x2, 0xa8, 0xa0, 
       0x3, 0x2, 0x2, 0x2, 0xa8, 0xa9, 0x3, 0x2, 0x2, 0x2, 0xa9, 0xaa, 0x3, 
       0x2, 0x2, 0x2, 0xaa, 0xac, 0x7, 0xa, 0x2, 0x2, 0xab, 0x9e, 0x3, 0x2, 
       0x2, 0x2, 0xab, 0x9f, 0x3, 0x2, 0x2, 0x2, 0xac, 0x17, 0x3, 0x2, 0x2, 
       0x2, 0xad, 0xae, 0x5, 0x1a, 0xe, 0x2, 0xae, 0xaf, 0x7, 0x24, 0x2, 
       0x2, 0xaf, 0xb1, 0x7, 0xd, 0x2, 0x2, 0xb0, 0xb2, 0x5, 0x1c, 0xf, 
       0x2, 0xb1, 0xb0, 0x3, 0x2, 0x2, 0x2, 0xb1, 0xb2, 0x3, 0x2, 0x2, 0x2, 
       0xb2, 0xb3, 0x3, 0x2, 0x2, 0x2, 0xb3, 0xb4, 0x7, 0xe, 0x2, 0x2, 0xb4, 
       0xb5, 0x5, 0x20, 0x11, 0x2, 0xb5, 0x19, 0x3, 0x2, 0x2, 0x2, 0xb6, 
       0xb7, 0x9, 0x3, 0x2, 0x2, 0xb7, 0x1b, 0x3, 0x2, 0x2, 0x2, 0xb8, 0xbd, 
       0x5, 0x1e, 0x10, 0x2, 0xb9, 0xba, 0x7, 0x4, 0x2, 0x2, 0xba, 0xbc, 
       0x5, 0x1e, 0x10, 0x2, 0xbb, 0xb9, 0x3, 0x2, 0x2, 0x2, 0xbc, 0xbf, 
       0x3, 0x2, 0x2, 0x2, 0xbd, 0xbb, 0x3, 0x2, 0x2, 0x2, 0xbd, 0xbe, 0x3, 
       0x2, 0x2, 0x2, 0xbe, 0x1d, 0x3, 0x2, 0x2, 0x2, 0xbf, 0xbd, 0x3, 0x2, 
       0x2, 0x2, 0xc0, 0xc1, 0x5, 0xa, 0x6, 0x2, 0xc1, 0xc2, 0x7, 0x24, 
       0x2, 0x2, 0xc2, 0xd1, 0x3, 0x2, 0x2, 0x2, 0xc3, 0xc4, 0x5, 0xa, 0x6, 
       0x2, 0xc4, 0xc5, 0x7, 0x24, 0x2, 0x2, 0xc5, 0xc6, 0x7, 0xb, 0x2, 
       0x2, 0xc6, 0xcd, 0x7, 0xc, 0x2, 0x2, 0xc7, 0xc8, 0x7, 0xb, 0x2, 0x2, 
       0xc8, 0xc9, 0x5, 0x26, 0x14, 0x2, 0xc9, 0xca, 0x7, 0xc, 0x2, 0x2, 
       0xca, 0xcc, 0x3, 0x2, 0x2, 0x2, 0xcb, 0xc7, 0x3, 0x2, 0x2, 0x2, 0xcc, 
       0xcf, 0x3, 0x2, 0x2, 0x2, 0xcd, 0xcb, 0x3, 0x2, 0x2, 0x2, 0xcd, 0xce, 
       0x3, 0x2, 0x2, 0x2, 0xce, 0xd1, 0x3, 0x2, 0x2, 0x2, 0xcf, 0xcd, 0x3, 
       0x2, 0x2, 0x2, 0xd0, 0xc0, 0x3, 0x2, 0x2, 0x2, 0xd0, 0xc3, 0x3, 0x2, 
       0x2, 0x2, 0xd1, 0x1f, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xd6, 0x7, 0x9, 0x2, 
       0x2, 0xd3, 0xd5, 0x5, 0x22, 0x12, 0x2, 0xd4, 0xd3, 0x3, 0x2, 0x2, 
       0x2, 0xd5, 0xd8, 0x3, 0x2, 0x2, 0x2, 0xd6, 0xd4, 0x3, 0x2, 0x2, 0x2, 
       0xd6, 0xd7, 0x3, 0x2, 0x2, 0x2, 0xd7, 0xd9, 0x3, 0x2, 0x2, 0x2, 0xd8, 
       0xd6, 0x3, 0x2, 0x2, 0x2, 0xd9, 0xda, 0x7, 0xa, 0x2, 0x2, 0xda, 0x21, 
       0x3, 0x2, 0x2, 0x2, 0xdb, 0xde, 0x5, 0x6, 0x4, 0x2, 0xdc, 0xde, 0x5, 
       0x24, 0x13, 0x2, 0xdd, 0xdb, 0x3, 0x2, 0x2, 0x2, 0xdd, 0xdc, 0x3, 
       0x2, 0x2, 0x2, 0xde, 0x23, 0x3, 0x2, 0x2, 0x2, 0xdf, 0xe0, 0x5, 0x2a, 
       0x16, 0x2, 0xe0, 0xe1, 0x7, 0x8, 0x2, 0x2, 0xe1, 0xe2, 0x5, 0x26, 
       0x14, 0x2, 0xe2, 0xe3, 0x7, 0x5, 0x2, 0x2, 0xe3, 0x102, 0x3, 0x2, 
       0x2, 0x2, 0xe4, 0xe6, 0x5, 0x26, 0x14, 0x2, 0xe5, 0xe4, 0x3, 0x2, 
       0x2, 0x2, 0xe5, 0xe6, 0x3, 0x2, 0x2, 0x2, 0xe6, 0xe7, 0x3, 0x2, 0x2, 
       0x2, 0xe7, 0x102, 0x7, 0x5, 0x2, 0x2, 0xe8, 0x102, 0x5, 0x20, 0x11, 
       0x2, 0xe9, 0xea, 0x7, 0x10, 0x2, 0x2, 0xea, 0xeb, 0x7, 0xd, 0x2, 
       0x2, 0xeb, 0xec, 0x5, 0x28, 0x15, 0x2, 0xec, 0xed, 0x7, 0xe, 0x2, 
       0x2, 0xed, 0xf0, 0x5, 0x24, 0x13, 0x2, 0xee, 0xef, 0x7, 0x11, 0x2, 
       0x2, 0xef, 0xf1, 0x5, 0x24, 0x13, 0x2, 0xf0, 0xee, 0x3, 0x2, 0x2, 
       0x2, 0xf0, 0xf1, 0x3, 0x2, 0x2, 0x2, 0xf1, 0x102, 0x3, 0x2, 0x2, 
       0x2, 0xf2, 0xf3, 0x7, 0x12, 0x2, 0x2, 0xf3, 0xf4, 0x7, 0xd, 0x2, 
       0x2, 0xf4, 0xf5, 0x5, 0x28, 0x15, 0x2, 0xf5, 0xf6, 0x7, 0xe, 0x2, 
       0x2, 0xf6, 0xf7, 0x5, 0x24, 0x13, 0x2, 0xf7, 0x102, 0x3, 0x2, 0x2, 
       0x2, 0xf8, 0xf9, 0x7, 0x13, 0x2, 0x2, 0xf9, 0x102, 0x7, 0x5, 0x2, 
       0x2, 0xfa, 0xfb, 0x7, 0x14, 0x2, 0x2, 0xfb, 0x102, 0x7, 0x5, 0x2, 
       0x2, 0xfc, 0xfe, 0x7, 0x15, 0x2, 0x2, 0xfd, 0xff, 0x5, 0x26, 0x14, 
       0x2, 0xfe, 0xfd, 0x3, 0x2, 0x2, 0x2, 0xfe, 0xff, 0x3, 0x2, 0x2, 0x2, 
       0xff, 0x100, 0x3, 0x2, 0x2, 0x2, 0x100, 0x102, 0x7, 0x5, 0x2, 0x2, 
       0x101, 0xdf, 0x3, 0x2, 0x2, 0x2, 0x101, 0xe5, 0x3, 0x2, 0x2, 0x2, 
       0x101, 0xe8, 0x3, 0x2, 0x2, 0x2, 0x101, 0xe9, 0x3, 0x2, 0x2, 0x2, 
       0x101, 0xf2, 0x3, 0x2, 0x2, 0x2, 0x101, 0xf8, 0x3, 0x2, 0x2, 0x2, 
       0x101, 0xfa, 0x3, 0x2, 0x2, 0x2, 0x101, 0xfc, 0x3, 0x2, 0x2, 0x2, 
       0x102, 0x25, 0x3, 0x2, 0x2, 0x2, 0x103, 0x104, 0x5, 0x3a, 0x1e, 0x2, 
       0x104, 0x27, 0x3, 0x2, 0x2, 0x2, 0x105, 0x106, 0x5, 0x42, 0x22, 0x2, 
       0x106, 0x29, 0x3, 0x2, 0x2, 0x2, 0x107, 0x112, 0x7, 0x24, 0x2, 0x2, 
       0x108, 0x10d, 0x7, 0x24, 0x2, 0x2, 0x109, 0x10a, 0x7, 0xb, 0x2, 0x2, 
       0x10a, 0x10b, 0x5, 0x26, 0x14, 0x2, 0x10b, 0x10c, 0x7, 0xc, 0x2, 
       0x2, 0x10c, 0x10e, 0x3, 0x2, 0x2, 0x2, 0x10d, 0x109, 0x3, 0x2, 0x2, 
       0x2, 0x10e, 0x10f, 0x3, 0x2, 0x2, 0x2, 0x10f, 0x10d, 0x3, 0x2, 0x2, 
       0x2, 0x10f, 0x110, 0x3, 0x2, 0x2, 0x2, 0x110, 0x112, 0x3, 0x2, 0x2, 
       0x2, 0x111, 0x107, 0x3, 0x2, 0x2, 0x2, 0x111, 0x108, 0x3, 0x2, 0x2, 
       0x2, 0x112, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x113, 0x114, 0x7, 0xd, 0x2, 
       0x2, 0x114, 0x115, 0x5, 0x26, 0x14, 0x2, 0x115, 0x116, 0x7, 0xe, 
       0x2, 0x2, 0x116, 0x11a, 0x3, 0x2, 0x2, 0x2, 0x117, 0x11a, 0x5, 0x2a, 
       0x16, 0x2, 0x118, 0x11a, 0x5, 0x2e, 0x18, 0x2, 0x119, 0x113, 0x3, 
       0x2, 0x2, 0x2, 0x119, 0x117, 0x3, 0x2, 0x2, 0x2, 0x119, 0x118, 0x3, 
       0x2, 0x2, 0x2, 0x11a, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x11b, 0x11e, 0x5, 
       0x46, 0x24, 0x2, 0x11c, 0x11e, 0x5, 0x48, 0x25, 0x2, 0x11d, 0x11b, 
       0x3, 0x2, 0x2, 0x2, 0x11d, 0x11c, 0x3, 0x2, 0x2, 0x2, 0x11e, 0x2f, 
       0x3, 0x2, 0x2, 0x2, 0x11f, 0x12a, 0x5, 0x2c, 0x17, 0x2, 0x120, 0x121, 
       0x7, 0x24, 0x2, 0x2, 0x121, 0x123, 0x7, 0xd, 0x2, 0x2, 0x122, 0x124, 
       0x5, 0x34, 0x1b, 0x2, 0x123, 0x122, 0x3, 0x2, 0x2, 0x2, 0x123, 0x124, 
       0x3, 0x2, 0x2, 0x2, 0x124, 0x125, 0x3, 0x2, 0x2, 0x2, 0x125, 0x12a, 
       0x7, 0xe, 0x2, 0x2, 0x126, 0x127, 0x5, 0x32, 0x1a, 0x2, 0x127, 0x128, 
       0x5, 0x30, 0x19, 0x2, 0x128, 0x12a, 0x3, 0x2, 0x2, 0x2, 0x129, 0x11f, 
       0x3, 0x2, 0x2, 0x2, 0x129, 0x120, 0x3, 0x2, 0x2, 0x2, 0x129, 0x126, 
       0x3, 0x2, 0x2, 0x2, 0x12a, 0x31, 0x3, 0x2, 0x2, 0x2, 0x12b, 0x12c, 
       0x9, 0x4, 0x2, 0x2, 0x12c, 0x33, 0x3, 0x2, 0x2, 0x2, 0x12d, 0x132, 
       0x5, 0x36, 0x1c, 0x2, 0x12e, 0x12f, 0x7, 0x4, 0x2, 0x2, 0x12f, 0x131, 
       0x5, 0x36, 0x1c, 0x2, 0x130, 0x12e, 0x3, 0x2, 0x2, 0x2, 0x131, 0x134, 
       0x3, 0x2, 0x2, 0x2, 0x132, 0x130, 0x3, 0x2, 0x2, 0x2, 0x132, 0x133, 
       0x3, 0x2, 0x2, 0x2, 0x133, 0x35, 0x3, 0x2, 0x2, 0x2, 0x134, 0x132, 
       0x3, 0x2, 0x2, 0x2, 0x135, 0x138, 0x5, 0x26, 0x14, 0x2, 0x136, 0x138, 
       0x7, 0x29, 0x2, 0x2, 0x137, 0x135, 0x3, 0x2, 0x2, 0x2, 0x137, 0x136, 
       0x3, 0x2, 0x2, 0x2, 0x138, 0x37, 0x3, 0x2, 0x2, 0x2, 0x139, 0x13e, 
       0x5, 0x30, 0x19, 0x2, 0x13a, 0x13b, 0x9, 0x5, 0x2, 0x2, 0x13b, 0x13d, 
       0x5, 0x30, 0x19, 0x2, 0x13c, 0x13a, 0x3, 0x2, 0x2, 0x2, 0x13d, 0x140, 
       0x3, 0x2, 0x2, 0x2, 0x13e, 0x13c, 0x3, 0x2, 0x2, 0x2, 0x13e, 0x13f, 
       0x3, 0x2, 0x2, 0x2, 0x13f, 0x39, 0x3, 0x2, 0x2, 0x2, 0x140, 0x13e, 
       0x3, 0x2, 0x2, 0x2, 0x141, 0x146, 0x5, 0x38, 0x1d, 0x2, 0x142, 0x143, 
       0x9, 0x6, 0x2, 0x2, 0x143, 0x145, 0x5, 0x38, 0x1d, 0x2, 0x144, 0x142, 
       0x3, 0x2, 0x2, 0x2, 0x145, 0x148, 0x3, 0x2, 0x2, 0x2, 0x146, 0x144, 
       0x3, 0x2, 0x2, 0x2, 0x146, 0x147, 0x3, 0x2, 0x2, 0x2, 0x147, 0x3b, 
       0x3, 0x2, 0x2, 0x2, 0x148, 0x146, 0x3, 0x2, 0x2, 0x2, 0x149, 0x14e, 
       0x5, 0x3a, 0x1e, 0x2, 0x14a, 0x14b, 0x9, 0x7, 0x2, 0x2, 0x14b, 0x14d, 
       0x5, 0x3a, 0x1e, 0x2, 0x14c, 0x14a, 0x3, 0x2, 0x2, 0x2, 0x14d, 0x150, 
       0x3, 0x2, 0x2, 0x2, 0x14e, 0x14c, 0x3, 0x2, 0x2, 0x2, 0x14e, 0x14f, 
       0x3, 0x2, 0x2, 0x2, 0x14f, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x150, 0x14e, 
       0x3, 0x2, 0x2, 0x2, 0x151, 0x156, 0x5, 0x3c, 0x1f, 0x2, 0x152, 0x153, 
       0x9, 0x8, 0x2, 0x2, 0x153, 0x155, 0x5, 0x3c, 0x1f, 0x2, 0x154, 0x152, 
       0x3, 0x2, 0x2, 0x2, 0x155, 0x158, 0x3, 0x2, 0x2, 0x2, 0x156, 0x154, 
       0x3, 0x2, 0x2, 0x2, 0x156, 0x157, 0x3, 0x2, 0x2, 0x2, 0x157, 0x3f, 
       0x3, 0x2, 0x2, 0x2, 0x158, 0x156, 0x3, 0x2, 0x2, 0x2, 0x159, 0x15e, 
       0x5, 0x3e, 0x20, 0x2, 0x15a, 0x15b, 0x7, 0x22, 0x2, 0x2, 0x15b, 0x15d, 
       0x5, 0x3e, 0x20, 0x2, 0x15c, 0x15a, 0x3, 0x2, 0x2, 0x2, 0x15d, 0x160, 
       0x3, 0x2, 0x2, 0x2, 0x15e, 0x15c, 0x3, 0x2, 0x2, 0x2, 0x15e, 0x15f, 
       0x3, 0x2, 0x2, 0x2, 0x15f, 0x41, 0x3, 0x2, 0x2, 0x2, 0x160, 0x15e, 
       0x3, 0x2, 0x2, 0x2, 0x161, 0x166, 0x5, 0x40, 0x21, 0x2, 0x162, 0x163, 
       0x7, 0x23, 0x2, 0x2, 0x163, 0x165, 0x5, 0x40, 0x21, 0x2, 0x164, 0x162, 
       0x3, 0x2, 0x2, 0x2, 0x165, 0x168, 0x3, 0x2, 0x2, 0x2, 0x166, 0x164, 
       0x3, 0x2, 0x2, 0x2, 0x166, 0x167, 0x3, 0x2, 0x2, 0x2, 0x167, 0x43, 
       0x3, 0x2, 0x2, 0x2, 0x168, 0x166, 0x3, 0x2, 0x2, 0x2, 0x169, 0x16a, 
       0x5, 0x3a, 0x1e, 0x2, 0x16a, 0x45, 0x3, 0x2, 0x2, 0x2, 0x16b, 0x16f, 
       0x7, 0x25, 0x2, 0x2, 0x16c, 0x16f, 0x7, 0x26, 0x2, 0x2, 0x16d, 0x16f, 
       0x7, 0x27, 0x2, 0x2, 0x16e, 0x16b, 0x3, 0x2, 0x2, 0x2, 0x16e, 0x16c, 
       0x3, 0x2, 0x2, 0x2, 0x16e, 0x16d, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x47, 
       0x3, 0x2, 0x2, 0x2, 0x170, 0x171, 0x7, 0x28, 0x2, 0x2, 0x171, 0x49, 
       0x3, 0x2, 0x2, 0x2, 0x29, 0x4e, 0x50, 0x57, 0x60, 0x6e, 0x77, 0x7a, 
       0x7d, 0x85, 0x93, 0x9c, 0xa5, 0xa8, 0xab, 0xb1, 0xbd, 0xcd, 0xd0, 
       0xd6, 0xdd, 0xe5, 0xf0, 0xfe, 0x101, 0x10f, 0x111, 0x119, 0x11d, 
       0x123, 0x129, 0x132, 0x137, 0x13e, 0x146, 0x14e, 0x156, 0x15e, 0x166, 
       0x16e, 
  };

  _serializedATN.insert(_serializedATN.end(), serializedATNSegment0,
    serializedATNSegment0 + sizeof(serializedATNSegment0) / sizeof(serializedATNSegment0[0]));


  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

SysYParserParser::Initializer SysYParserParser::_init;
