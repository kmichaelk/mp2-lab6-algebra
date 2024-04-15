parser grammar AlgebraicExpressionParser;

options {
    tokenVocab = AlgebraicExpressionLexer;
}

input
    : polynomial
    ;

polynomial
    : polynomial MUL   polynomial               # expressionMul
    | polynomial DIV   polynomial               # expressionDiv
    | polynomial PLUS  polynomial               # expressionSum
    | polynomial MINUS polynomial               # expressionDiff
    | LPAREN polynomial RPAREN                  # group
    | ( ( PLUS | MINUS ) subject )* subject     # atomicGroup
    ;

subject
    : monomial
    | variable
    ;

monomial
    : coefficient? atom+
    | coefficient
    ;

atom
    : atomicVariable ( POW coefficient )?
    ;

coefficient
    : COEFFICIENT
    ;

atomicVariable
    : ATOMIC_VARIABLE
    ;

variable : VARIABLE;
