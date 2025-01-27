#pragma once

#include "./assembler/yas.h"

#include <vector>

namespace YAS
{
   // A helper class of Lexer class
   class InstrLexer
   {
   public:
      explicit InstrLexer(Lexer &lex);
      int decode(const char *name);

   private:
      void getReg(int codepos, int hi);
      void getMem(int codepos);
      void getNum(int codepos, int bytes, int offset);

   private:
      Lexer &m_lex;
      std::vector<char> m_decodeBuf;
   };

}
