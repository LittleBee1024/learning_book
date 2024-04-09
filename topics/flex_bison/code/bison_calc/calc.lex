%{
#include "calc.h"
#include "stdlib.h"

// Has to define YYSTYPE before "hcl_yacc_gen.hpp"
#define YYSTYPE NodePtr
#include "calc_yacc_gen.hpp"

extern YYSTYPE calc_lval;
#define YY_DECL int calc_lex(Calc *calc)
%}

%option noinput
%option nounput
%option noyywrap
%option yylineno

EXP ([Ee][-+]?[0-9]+)

%%

[ \r\t\f]               ;
"+"                     { return ADD; }
"-"                     { return SUB; }
"*"                     { return MUL; }
"/"                     { return DIV; }
"("                     { return LBRACE; }
")"                     { return RBRACE; }
[0-9]+"."[0-9]*{EXP}?   |
"."?[0-9]+{EXP}?        { calc_lval = calc->createNode(t_NUM, nullptr, nullptr, atof(calc_text)); return NUM; }
\n                      { return EOL; }
.                       { calc->fail("Bad input character '%s'", calc_text); }
