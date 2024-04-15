lexer grammar AlgebraicExpressionLexer;

ATOMIC_VARIABLE
    : [x-z]
    ;

COEFFICIENT
    : NUMBER (UNSIGNED_INTEGER)?
    ;

fragment NUMBER
    : ('0' .. '9')+ ('.' ('0' .. '9')+)?
    ;

fragment UNSIGNED_INTEGER
    : ('0' .. '9')+
    ;

fragment SIGN
    : '+'
    | '-'
    ;

LPAREN : '(' ;
RPAREN : ')' ;
//
PLUS   : '+' ;
MINUS  : '-' ;
//
MUL     : '*' ;
DIV    : '/' ;
//
POINT  : '.' ;
POW    : '^' ;

VARIABLE
    : [A-Z]+
    ;

WS
    : [ \r\n\t]+ -> skip
    ;
