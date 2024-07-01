parser grammar SysYParser;

options {
    tokenVocab = SysYLexer;//注意使用该语句指定词法分析器；请不要修改词法分析器或语法分析器的文件名，否则Makefile可能无法正常工作，影响评测结果
}

program
   : compUnit
   ;
compUnit
   : singlecompUnit+ EOF
   ;

singlecompUnit: (funcDef | decl);
decl : constDecl | varDecl;
constDecl : CONST bType constDef (COMMA constDef )* SEMICOLON;
bType : INT;
constDef : IDENT ( L_BRACKT constExp R_BRACKT )* ASSIGN constInitVal;
constInitVal : constExp | multipalInitVal;
multipalInitVal: L_BRACE ( constInitVal ( COMMA constInitVal )* )? R_BRACE;
varDecl : bType varDef ( COMMA varDef )*SEMICOLON;
varDef : IDENT ( L_BRACKT constExp R_BRACKT )*
| IDENT ( L_BRACKT constExp R_BRACKT )* ASSIGN initVal;

initVal : exp | L_BRACE ( initVal ( COMMA initVal )* )? R_BRACE;
funcDef : funcType IDENT L_PAREN (funcFParam ( COMMA funcFParam )*)? R_PAREN block;
funcType : VOID | INT;
funcFParam : bType IDENT (L_BRACKT R_BRACKT ( L_BRACKT exp R_BRACKT )*)*;
block : L_BRACE ( blockItem )* R_BRACE;
blockItem : decl | stmt;
stmt : assgin_stmt | single_exps | block
|if_stmt
| while_stmt
| break_stmt | continue_stmt
| return_stmt;
assgin_stmt:lVal ASSIGN exp SEMICOLON;
single_exps:(exp)?SEMICOLON;
if_stmt:IF L_PAREN cond R_PAREN stmt (else_if_stmt)* (else_stmt)?;
while_stmt:WHILE L_PAREN cond R_PAREN stmt;
break_stmt:BREAK SEMICOLON;
continue_stmt:CONTINUE SEMICOLON;
return_stmt:RETURN (exp)? SEMICOLON;
else_stmt:ELSE stmt;
else_if_stmt:ELSE IF L_PAREN cond R_PAREN stmt;

exp
   : paren_exp
   | lVal
   | number
   | function_calling
   | unaryOp exp
   | exp (MUL | DIV | MOD) exp
   | exp (PLUS | MINUS) exp
   ;
unaryOp:(PLUS|MINUS| NOT);

function_calling:IDENT L_PAREN funcRParams? R_PAREN;


cond
   : exp
   | cond (LT | GT | LE | GE) cond
   | cond (EQ | NEQ) cond
   | cond AND cond
   | cond OR cond
   ;



lVal
   : IDENT (L_BRACKT exp R_BRACKT)*
   ;
paren_exp:L_PAREN exp R_PAREN;

number
   : INTEGER_CONST
   ;


funcRParams
   : param (COMMA param)*
   ;

param
   : exp
   ;

constExp
   : exp
   ;
