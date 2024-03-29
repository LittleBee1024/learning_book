%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calc.h"

#define YYSTYPE NodePtr
extern int calc_lex(Calc*);
extern int calc_lineno;

void calc_error(Calc *calc, const char *str)
{
   fprintf(stderr, "Error at line %d: %s\n", calc_lineno, str);
}

%}

%lex-param { Calc *calc }
%parse-param { Calc *calc }

%token ADD SUB MUL DIV LBRACE RBRACE NUM EOL

/* All operators are left associative. Listed from lowest to highest */
%left ADD SUB
%left MUL DIV

%%

calclist: /* empty */
   | calclist exp EOL   { calc->evalExpr($2); }
   | calclist EOL       ;
   ;

exp: factor
   | exp ADD factor { $$ = calc->createNode(t_ADD, $1, $3); }
   | exp SUB factor { $$ = calc->createNode(t_SUB, $1, $3); }
   ;

factor: term
   | factor MUL term { $$ = calc->createNode(t_MUL, $1, $3); }
   | factor DIV term { $$ = calc->createNode(t_DIV, $1, $3); }
   ;

term: NUM { $$ = $1; }
   | LBRACE exp RBRACE { $$ = $2; }
   | SUB term { $$ = calc->createNode(t_MINUS, $2, nullptr); }
   ;

%%

