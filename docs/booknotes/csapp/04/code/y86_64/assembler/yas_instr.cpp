#include "./yas_instr.h"

#include "isa.h"

#include <string.h>
#include <string>

namespace
{
   struct Instr
   {
      const char *name;
      unsigned char code; /* Byte code for instruction+op */
      int bytes;
      INSTR_ARG arg1;
      int arg1pos;
      int arg1hi; /* 0/1 for register argument, # bytes for allocation */
      INSTR_ARG arg2;
      int arg2pos;
      int arg2hi; /* 0/1 */
   };

   const std::vector<Instr> Y86_64_INSTR = {
       {"nop", HPACK(I_NOP, F_NONE), 1, NO_ARG, 0, 0, NO_ARG, 0, 0},
       {"halt", HPACK(I_HALT, F_NONE), 1, NO_ARG, 0, 0, NO_ARG, 0, 0},
       {"rrmovq", HPACK(I_RRMOVQ, F_NONE), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       /* Conditional move instructions are variants of RRMOVQ */
       {"cmovle", HPACK(I_RRMOVQ, C_LE), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmovl", HPACK(I_RRMOVQ, C_L), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmove", HPACK(I_RRMOVQ, C_E), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmovne", HPACK(I_RRMOVQ, C_NE), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmovge", HPACK(I_RRMOVQ, C_GE), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmovg", HPACK(I_RRMOVQ, C_G), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       /* arg1hi indicates number of bytes */
       {"irmovq", HPACK(I_IRMOVQ, F_NONE), 10, I_ARG, 2, 8, R_ARG, 1, 0},
       {"rmmovq", HPACK(I_RMMOVQ, F_NONE), 10, R_ARG, 1, 1, M_ARG, 1, 0},
       {"mrmovq", HPACK(I_MRMOVQ, F_NONE), 10, M_ARG, 1, 0, R_ARG, 1, 1},
       {"addq", HPACK(I_ALU, A_ADD), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"subq", HPACK(I_ALU, A_SUB), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"andq", HPACK(I_ALU, A_AND), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"xorq", HPACK(I_ALU, A_XOR), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       /* arg1hi indicates number of bytes */
       {"jmp", HPACK(I_JMP, C_YES), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jle", HPACK(I_JMP, C_LE), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jl", HPACK(I_JMP, C_L), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"je", HPACK(I_JMP, C_E), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jne", HPACK(I_JMP, C_NE), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jge", HPACK(I_JMP, C_GE), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jg", HPACK(I_JMP, C_G), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"call", HPACK(I_CALL, F_NONE), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"ret", HPACK(I_RET, F_NONE), 1, NO_ARG, 0, 0, NO_ARG, 0, 0},
       {"pushq", HPACK(I_PUSHQ, F_NONE), 2, R_ARG, 1, 1, NO_ARG, 0, 0},
       {"popq", HPACK(I_POPQ, F_NONE), 2, R_ARG, 1, 1, NO_ARG, 0, 0},
       {"iaddq", HPACK(I_IADDQ, F_NONE), 10, I_ARG, 2, 8, R_ARG, 1, 0},
       /* this is just a hack to make the I_POP2 code have an associated name */
       {"pop2", HPACK(I_POP2, F_NONE), 0, NO_ARG, 0, 0, NO_ARG, 0, 0},
       /* For allocation instructions, arg1hi indicates number of bytes */
       {".byte", 0x00, 1, I_ARG, 0, 1, NO_ARG, 0, 0},
       {".word", 0x00, 2, I_ARG, 0, 2, NO_ARG, 0, 0},
       {".long", 0x00, 4, I_ARG, 0, 4, NO_ARG, 0, 0},
       {".quad", 0x00, 8, I_ARG, 0, 8, NO_ARG, 0, 0},
       /* empty instruction */
       {nullptr, 0, 0, NO_ARG, 0, 0, NO_ARG, 0, 0}};

   const Instr &findInstr(const char *name)
   {
      for (const auto &instr : Y86_64_INSTR)
      {
         if (strcmp(instr.name, name) == 0)
            return instr;
      }
      return Y86_64_INSTR.back(); // invalid instruction
   }

   const std::vector<std::string> Y86_64_REG_NAMES = {"%rax", "%rcx", "%rdx", "%rbx", "%rsp", "%rbp", "%rsi", "%rdi",
                                                      "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"};

   REG_ID findRegister(const char *name)
   {
      for (size_t i = 0; i < Y86_64_REG_NAMES.size(); i++)
      {
         if (Y86_64_REG_NAMES[i].compare(name) == 0)
            return (REG_ID)i;
      }
      return REG_ERR;
   }
}

namespace YAS
{

   InstrLexer::InstrLexer(Lexer &lex) : m_lex(lex)
   {
   }

   int InstrLexer::decode(const char *name)
   {
      m_decodeBuf.clear();

      const auto &instr = findInstr(name);
      if (instr.name == nullptr)
      {
         m_lex.fail("Invalid Instruction");
         return ERROR;
      }
      // get expected instruction token, pop it from the deque
      m_lex.m_tokens.pop_front();

      // don't process instruction in pass 1
      if (m_lex.m_pass == 1)
      {
         m_lex.m_addr += instr.bytes;
         return DONE;
      }

      // process the instructions
      m_decodeBuf.resize(instr.bytes, 0);
      m_decodeBuf[0] = instr.code;
      m_decodeBuf[1] = static_cast<char>(HPACK(REG_NONE, REG_NONE));
      switch (instr.arg1)
      {
      case R_ARG:
         getReg(instr.arg1pos, instr.arg1hi);
         break;
      case M_ARG:
         getMem(instr.arg1pos);
         break;
      case I_ARG:
         getNum(instr.arg1pos, instr.arg1hi, 0);
         break;
      case NO_ARG:
      default:
         break;
      }

      if (instr.arg2 != NO_ARG)
      {
         /* Get comma  */
         if (m_lex.m_tokens.front().type != TOK_PUNCT || m_lex.m_tokens.front().cval != ',')
         {
            m_lex.fail("Expecting Comma");
            return ERROR;
         }
         m_lex.m_tokens.pop_front();

         /* Get second argument */
         switch (instr.arg2)
         {
         case R_ARG:
            getReg(instr.arg2pos, instr.arg2hi);
            break;
         case M_ARG:
            getMem(instr.arg2pos);
            break;
         case I_ARG:
            getNum(instr.arg2pos, instr.arg2hi, 0);
            break;
         case NO_ARG:
         default:
            break;
         }
      }

      m_lex.dumpCode(m_decodeBuf);
      m_lex.m_addr += instr.bytes;
      return DONE;
   }

   /* Parse Register from set of m_impl.tokens and put into high or low
   4 bits of code[codepos] */
   void InstrLexer::getReg(int codepos, int hi)
   {
      int rval = REG_NONE;
      char c;
      if (m_lex.m_tokens.front().type != TOK_REG)
      {
         m_lex.fail("Expecting Register ID");
         return;
      }

      rval = findRegister(m_lex.m_tokens.front().sval.c_str());
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
   void InstrLexer::getMem(int codepos)
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

            rval = findRegister(m_lex.m_tokens.front().sval.c_str());
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
   void InstrLexer::getNum(int codepos, int bytes, int offset)
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
