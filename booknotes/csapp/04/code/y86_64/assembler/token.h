#pragma once

#include "./_common/isa.h"
#include <string>

namespace YAS
{

   typedef enum
   {
      TOK_IDENT,  // user defined symbol name
      TOK_NUM,    // decimal or hex number
      TOK_REG,    // Y86-64 register name
      TOK_INSTR,  // Y86-64 instruction icode
      TOK_PUNCT,  // one character splits tockens
      TOK_ERR     // invalid token
   } TokenType;

   struct Token
   {
      Token(TokenType t, const char *s, word_t i, char c) : type(t),
                                                            sval(s ? s : ""),
                                                            ival(i),
                                                            cval(c)
      {
      }

      TokenType type;
      std::string sval;
      word_t ival;
      char cval;
   };

}
