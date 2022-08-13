%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hcl.h"

#define YYSTYPE HCL::NodePtr
extern int yylex(HCL::Parser*);

void yyerror(HCL::Parser *par, const char *str)
{
   static const int ERRLIM = 5;
   static int errcnt = 0;
   fprintf(stderr, "Error: near line %d: %s\n", par->getLineNum(), str);
   if (++errcnt > ERRLIM)
   {
      fprintf(stderr, "Too many errors, aborting\n");
      exit(1);
   }
}

%}

%lex-param { HCL::Parser *par }
%parse-param { HCL::Parser *par }

%token QUOTE BOOLARG BOOL WORDARG WORD QSTRING
       VAR NUM ASSIGN SEMI COLON COMMA LPAREN RPAREN LBRACE
       RBRACE LBRACK RBRACK AND OR NOT COMP IN

/* All operators are left associative.  Listed from lowest to highest */
%left OR
%left AND
%left NOT
%left COMP
%left IN

%%

statements: /* empty */
      | statements statement
      ;

statement:
      QUOTE QSTRING                       { par->outQuoteCode($2); }
      | BOOLARG VAR QSTRING               { par->addSymbol($2, $3, 1); }
      | WORDARG VAR QSTRING               { par->addSymbol($2, $3, 0); }
      | BOOL VAR ASSIGN expr SEMI         { par->outFunction($2, $4, 1); }
      | WORD VAR ASSIGN expr SEMI         { par->outFunction($2, $4, 0); }
      ;

expr:
      VAR                                 { $$=$1; }
      | NUM                               { $$=$1; }
      | LPAREN expr RPAREN                { $$=$2; }
      | NOT expr                          { $$=par->createNotExpr($2); }
      | expr AND expr                     { $$=par->createAndExpr($1, $3); }
      | expr OR expr                      { $$=par->createOrExpr($1, $3); }
      | expr COMP expr                    { $$=par->createCompExpr($2,$1,$3); }
      | expr IN LBRACE exprlist RBRACE    { $$=par->createEleExpr($1, $4);}
      | LBRACK caselist RBRACK            { $$=$2; }
      ;

exprlist:
      expr                                { $$=$1; }
      | exprlist COMMA expr               { $$=par->concat($1, $3); }
      ;

caselist:
      /* Empty */                         { $$=NULL; }
      | caselist expr COLON expr SEMI     { $$=par->concat($1, par->createCaseExpr($2, $4));}
