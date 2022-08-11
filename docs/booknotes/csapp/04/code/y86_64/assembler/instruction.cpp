#include "./instruction.h"

namespace YAS
{

   Instruction::Instruction(Lexer &lex) : m_lex(lex)
   {
   }

   int Instruction::decode(const char *name)
   {
      m_decodeBuf.clear();

      instr_ptr instr = find_instr(name);
      if (instr == nullptr)
      {
         m_lex.fail("Invalid Instruction");
         return ERROR;
      }
      // get expected instruction token, pop it from the deque
      m_lex.m_tokens.pop_front();

      // don't process instruction in pass 1
      if (m_lex.m_pass == 1)
      {
         m_lex.m_addr += instr->bytes;
         return DONE;
      }

      // process the instructions
      m_decodeBuf.resize(instr->bytes, 0);
      m_decodeBuf[0] = instr->code;
      m_decodeBuf[1] = static_cast<char>(HPACK(REG_NONE, REG_NONE));
      switch (instr->arg1)
      {
      case R_ARG:
         getReg(instr->arg1pos, instr->arg1hi);
         break;
      case M_ARG:
         getMem(instr->arg1pos);
         break;
      case I_ARG:
         getNum(instr->arg1pos, instr->arg1hi, 0);
         break;
      case NO_ARG:
      default:
         break;
      }

      if (instr->arg2 != NO_ARG)
      {
         /* Get comma  */
         if (m_lex.m_tokens.front().type != TOK_PUNCT || m_lex.m_tokens.front().cval != ',')
         {
            m_lex.fail("Expecting Comma");
            return ERROR;
         }
         m_lex.m_tokens.pop_front();

         /* Get second argument */
         switch (instr->arg2)
         {
         case R_ARG:
            getReg(instr->arg2pos, instr->arg2hi);
            break;
         case M_ARG:
            getMem(instr->arg2pos);
            break;
         case I_ARG:
            getNum(instr->arg2pos, instr->arg2hi, 0);
            break;
         case NO_ARG:
         default:
            break;
         }
      }

      m_lex.dumpCode(m_decodeBuf);
      m_lex.m_addr += instr->bytes;
      return DONE;
   }

   /* Parse Register from set of m_impl.tokens and put into high or low
   4 bits of code[codepos] */
   void Instruction::getReg(int codepos, int hi)
   {
      int rval = REG_NONE;
      char c;
      if (m_lex.m_tokens.front().type != TOK_REG)
      {
         m_lex.fail("Expecting Register ID");
         return;
      }

      rval = find_register(m_lex.m_tokens.front().sval.c_str());
      m_lex.m_tokens.pop_front();

      /* Insert into output */
      c = m_decodeBuf[codepos];
      if (hi)
         c = (c & 0x0F) | (rval << 4);
      else
         c = (c & 0xF0) | rval;
      m_decodeBuf[codepos] = c;
   }

   /* Get memory reference.
   Can be of form:
   Num(Reg)
   (Reg)
   Num
   Ident
   Ident(Reg)
   Put Reg in low position of current byte, and Number in following bytes
   */
   void Instruction::getMem(int codepos)
   {
      char rval = REG_NONE;
      word_t val = 0;
      int i;
      char c;
      TokenType type = m_lex.m_tokens.front().type;
      /* Deal with optional displacement */
      if (type == TOK_NUM)
      {
         m_lex.m_tokens.pop_front();
         val = m_lex.m_tokens.front().ival;
         type = m_lex.m_tokens.front().type;
      }
      else if (type == TOK_IDENT)
      {
         val = m_lex.findSymbol(m_lex.m_tokens.front().sval.c_str());
         m_lex.m_tokens.pop_front();
         type = m_lex.m_tokens.front().type;
      }
      /* Check for optional register */
      if (type == TOK_PUNCT)
      {
         if (m_lex.m_tokens.front().cval == '(')
         {
            m_lex.m_tokens.pop_front();
            if (m_lex.m_tokens.front().type != TOK_REG)
            {
               m_lex.fail("Expecting Register Id");
               return;
            }

            rval = find_register(m_lex.m_tokens.front().sval.c_str());
            m_lex.m_tokens.pop_front();
            if (m_lex.m_tokens.front().type != TOK_PUNCT || m_lex.m_tokens.front().cval != ')')
            {
               m_lex.fail("Expecting ')'");
               return;
            }
            m_lex.m_tokens.pop_front();
         }
      }
      c = (m_decodeBuf[codepos] & 0xF0) | (rval & 0xF);
      m_decodeBuf[codepos++] = c;
      for (i = 0; i < 8; i++)
         m_decodeBuf[codepos + i] = (val >> (i * 8)) & 0xFF;
   }

   /* Get numeric value of given number of bytes */
   /* Offset indicates value to subtract from number (for PC relative) */
   void Instruction::getNum(int codepos, int bytes, int offset)
   {
      word_t val = 0;
      int i;
      if (m_lex.m_tokens.front().type == TOK_NUM)
      {
         val = m_lex.m_tokens.front().ival;
      }
      else if (m_lex.m_tokens.front().type == TOK_IDENT)
      {
         val = m_lex.findSymbol(m_lex.m_tokens.front().sval.c_str());
      }
      else
      {
         m_lex.fail("Number Expected");
         return;
      }
      m_lex.m_tokens.pop_front();

      val -= offset;
      for (i = 0; i < bytes; i++)
         m_decodeBuf[codepos + i] = (val >> (i * 8)) & 0xFF;
   }

}
