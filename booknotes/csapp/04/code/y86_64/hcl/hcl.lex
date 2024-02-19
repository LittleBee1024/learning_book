/* Grammar for Y86-64 HCL */

%{
#include "./hcl/hcl.h"
#include "./hcl/node.h"

// Has to define YYSTYPE before "hcl_yacc_gen.hpp"
#define YYSTYPE HCL::NodePtr
#include "hcl_yacc_gen.hpp"

extern YYSTYPE hcl_lval;
#define YY_DECL int hcl_lex(HCL::Parser *par)
%}

%option noinput
%option nounput
%option noyywrap

%%

[ \r\t\f]             ;                   // no action
[\n]                  par->finishLine();  // end of normal line
"#".*\n               par->finishLine();  // end of coment line
quote                 return(QUOTE);      // HCL keyword, token type can indicate the meaning because it is unique, no need hcl_lval in bison
boolsig               return(BOOLARG);
bool                  return(BOOL);
wordsig               return(WORDARG);
word                  return(WORD);
in                    return(IN);
'[^']*'               hcl_lval = par->createQuote(hcl_text); return(QSTRING); // create node in hcl_lval for bison for the quote value
[a-zA-Z][a-zA-Z0-9_]* hcl_lval = par->createVar(hcl_text); return(VAR);    // bison can get var name from hcl_lval
[0-9][0-9]*           hcl_lval = par->createNum(hcl_text); return(NUM);    // bison can get number from hcl_lval
-[0-9][0-9]*          hcl_lval = par->createNum(hcl_text); return(NUM);
"="                   return(ASSIGN);
";"                   return(SEMI);
":"                   return(COLON);
","                   return(COMMA);
"("                   return(LPAREN);
")"                   return(RPAREN);
"{"                   return(LBRACE);
"}"                   return(RBRACE);
"["                   return(LBRACK);
"]"                   return(RBRACK);
"&&"                  return(AND);
"||"                  return(OR);
"!="                  hcl_lval = par->createCompOp(hcl_text); return(COMP);   // it has several COMP node, so it needs to pass the contents to bison through hcl_lval
"=="                  hcl_lval = par->createCompOp(hcl_text); return(COMP);
"<"                   hcl_lval = par->createCompOp(hcl_text); return(COMP);
"<="                  hcl_lval = par->createCompOp(hcl_text); return(COMP);
">"                   hcl_lval = par->createCompOp(hcl_text); return(COMP);
">="                  hcl_lval = par->createCompOp(hcl_text); return(COMP);
"!"                   return(NOT);

%%
