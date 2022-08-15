
%{
#include "lexer.h"
#define YY_DECL int c_wclex(Lexer *lex)   // flex C API is wrapped Lexer class
%}

%option noyywrap
%option nounput

LineCommentStart     \/\/
BlockCommentStart    \/\*
BlockCommentEnd      \*\/
If                   if
Loop                 for|while
NewLine              \n
Blank                [ \t]

%x LINE_CMT
%x BLOCK_CMT

%%

<INITIAL>{LineCommentStart}            { BEGIN LINE_CMT; }
<INITIAL>{BlockCommentStart}           { BEGIN BLOCK_CMT; }

<INITIAL>{If}                          { lex->upIfNum(); }
<INITIAL>{Loop}                        { lex->upLoopNum(); }

<INITIAL>^{NewLine}                    |
<INITIAL>^{Blank}*{NewLine}            { lex->upEmptyLineNum(); }
<INITIAL>{NewLine}                     { lex->upCodeLineNum(); }

<LINE_CMT>{NewLine}                    { lex->upCommentLineNum(); BEGIN INITIAL; }

<BLOCK_CMT>{BlockCommentEnd}{NewLine}? { lex->upCommentLineNum(); BEGIN INITIAL; }
<BLOCK_CMT>{NewLine}                   { lex->upCommentLineNum(); }

<INITIAL,LINE_CMT,BLOCK_CMT>.          ; // any character (byte) except newline

%%

