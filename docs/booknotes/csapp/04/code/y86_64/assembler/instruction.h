#pragma once

#include "./yas.h"
#include "isa.h"

#include <vector>

namespace YAS
{
   // A helper class of Lexer class
   class Instruction
   {
   public:
      explicit Instruction(Lexer &lex);
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
