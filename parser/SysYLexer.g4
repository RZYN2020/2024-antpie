lexer grammar SysYLexer;
CONST : 'const';

INT : 'int';

VOID : 'void';

IF : 'if';

ELSE : 'else';

WHILE : 'while';

BREAK : 'break';

CONTINUE : 'continue';

RETURN : 'return';

PLUS : '+';

MINUS : '-';

MUL : '*';

DIV : '/';

MOD : '%';

ASSIGN : '=';

EQ : '==';

NEQ : '!=';

LT : '<';

GT : '>';

LE : '<=';

GE : '>=';

NOT : '!';

AND : '&&';

OR : '||';

L_PAREN : '(';

R_PAREN : ')';

L_BRACE : '{';

R_BRACE : '}';

L_BRACKT : '[';

R_BRACKT : ']';

COMMA : ',';

SEMICOLON : ';';

IDENT : ('_'|[a-zA-Z])('_'|[a-zA-Z]|[0-9])*
   ;

INTEGER_CONST :'0'|
DEMICAL|OCTCAL|HEXO
   ;

DEMICAL:([1-9])([0-9])*;
OCTCAL:'0'[0-7][0-7]*;
HEXO:('0x'|'0X')[0-9a-fA-F][0-9a-fA-F]*;

WS: [ \r\n\t]+->skip
   ;

LINE_COMMENT
   : '//' .*? '\n'->skip
   ;

MULTILINE_COMMENT
   : '/*' .*? '*/'->skip
   ;
