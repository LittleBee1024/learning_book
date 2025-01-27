/* Grammar for Y86-64 Assembler */

%{
#include "./assembler/yas.h"
#define YY_DECL int yas_lex(YAS::Lexer *lex)
%}

%option noinput
%option nounput
%option noyywrap

Instr         rrmovq|cmovle|cmovl|cmove|cmovne|cmovge|cmovg|rmmovq|mrmovq|irmovq|addq|subq|andq|xorq|jmp|jle|jl|je|jne|jge|jg|call|ret|pushq|popq|"."byte|"."word|"."long|"."quad|"."pos|"."align|halt|nop|iaddq
Letter        [a-zA-Z]
Digit         [0-9]
Ident         {Letter}({Letter}|{Digit}|_)*
Hex           [0-9a-fA-F]
Blank         [ \t]
Newline       [\n\r]
Return        [\r]
Char          [^\n\r]
Reg           %rax|%rcx|%rdx|%rbx|%rsi|%rdi|%rsp|%rbp|%r8|%r9|%r10|%r11|%r12|%r13|%r14

/* ERR condition is started if no token is matched */
%x ERR
%%

^{Char}*{Return}*{Newline}    {lex->saveLine(yas_text); REJECT;}  /* save non-comment line, and continue matching because of REJECT */

#{Char}*{Return}*{Newline}    {lex->processLine();}            /* encounter comment line, process tokens in last non-comment line */
"//"{Char}*{Return}*{Newline} {lex->processLine();}            /* encounter comment line, process tokens in last non-comment line */
"/*"{Char}*{Return}*{Newline} {lex->processLine();}            /* encounter comment line, process tokens in last non-comment line */
{Blank}*{Return}*{Newline}    {lex->processLine();}            /* encounter end-of-line flag , process tokens in last non-comment line */

{Blank}+                      ;
"$"+                          ;                                /* constant number start flag */
{Instr}                       lex->addInstr(yas_text);         /* Y86-64 instruction icode */
{Reg}                         lex->addReg(yas_text);           /* Y86-64 register name */
[-]?{Digit}+                  lex->addNum(atoll(yas_text));    /* positive/negative decimal number */
"0"[xX]{Hex}+                 lex->addNum(strtoull(yas_text, nullptr, 16));   /* hex number */
[():,]                        lex->addPunct(*yas_text);        /* one character splits tockens */
{Ident}                       lex->addIdent(yas_text);         /* user defined symbol name */
{Char}                        {; BEGIN ERR;}                   /* no match */
<ERR>{Char}*{Newline}         {lex->fail("Invalid line"); BEGIN 0;}           /* report error */
%%
