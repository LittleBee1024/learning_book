/* Grammar for Y86-64 HCL */

%{
#include "hcl.h"
#include "node.h"

// Has to define YYSTYPE before "hcl_yacc_gen.hpp"
#define YYSTYPE HCL::NodePtr
#include "hcl_yacc_gen.hpp"

extern YYSTYPE yylval;
#define YY_DECL int yylex(HCL::Parser *par)
%}

%option noinput
%option nounput
%option noyywrap

%%

[ \r\t\f]             ;                   // no action
[\n]                  par->finishLine();  // end of normal line
"#".*\n               par->finishLine();  // end of coment line
quote                 return(QUOTE);      // HCL keyword, token type can indicate the meaning because it is unique, no need yylval in bison
boolsig               return(BOOLARG);
bool                  return(BOOL);
wordsig               return(WORDARG);
word                  return(WORD);
in                    return(IN);
'[^']*'               yylval = par->createQuote(yytext); return(QSTRING); // create node in yylval for bison for the quote value
[a-zA-Z][a-zA-Z0-9_]* yylval = par->createVar(yytext); return(VAR);    // bison can get var name from yylval
[0-9][0-9]*           yylval = par->createNum(yytext); return(NUM);    // bison can get number from yylval
-[0-9][0-9]*          yylval = par->createNum(yytext); return(NUM);
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
"!="                  yylval = par->createCompOp(yytext); return(COMP);   // it has several COMP node, so it needs to pass the contents to bison through yylval
"=="                  yylval = par->createCompOp(yytext); return(COMP);
"<"                   yylval = par->createCompOp(yytext); return(COMP);
"<="                  yylval = par->createCompOp(yytext); return(COMP);
">"                   yylval = par->createCompOp(yytext); return(COMP);
">="                  yylval = par->createCompOp(yytext); return(COMP);
"!"                   return(NOT);

%%
