#include "./yas.h"
#include "./instruction.h"

#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************
 * Variables and functions from flex
 ***************************************/
// lexer input handler
extern FILE *yasin;
// main lexer function generated by flex with "yas.lex"
extern int yaslex(YAS::Lexer *);

namespace
{

   void hexstuff(char *dest, word_t value, int len)
   {
      int i;
      for (i = 0; i < len; i++)
      {
         char c;
         int h = (value >> 4 * i) & 0xF;
         c = h < 10 ? h + '0' : h - 10 + 'a';
         dest[len - i - 1] = c;
      }
   }

}

namespace YAS
{
   Lexer::Lexer(std::unique_ptr<InputInterface> &&in) : m_in(std::move(in)), m_pass(0), m_hitError(false),
                                                        m_lineno(0), m_addr(0)
   {
      // yasin is a global variable defined in flex
      assert(m_in->getYasIn() != nullptr);
      yasin = m_in->getYasIn();
   }

   int Lexer::parse(std::unique_ptr<OutputInterface> &&out)
   {
      m_out = std::move(out);

      m_pass = 1;
      resetYasIn();
      yaslex(this);
      if (m_hitError)
         return ERROR;

      m_pass = 2;
      resetYasIn();
      yaslex(this);
      return SUCCESS;
   }

   void Lexer::saveLine(const char *s)
   {
      if (s == nullptr)
         return;

      m_line = s;
      for (size_t i = m_line.size() - 1; m_line[i] == '\n' || m_line[i] == '\r'; i--)
         m_line[i] = '\0';
      m_lineno++;
   }

   void Lexer::processLine()
   {
      if (m_hitError)
         return reset();

      // Process different types of the line, only stop when it is done or it has an error
      if (processEmptyLine() != CONTINUE)
         return reset();

      if (processLabel() != CONTINUE)
         return reset();

      if (processPosInstr() != CONTINUE)
         return reset();

      if (processAlignInstr() != CONTINUE)
         return reset();

      if (processNormalInstr() != CONTINUE)
         return reset();

      fail("Has unprocess tokens");
      reset();
   }

   void Lexer::addInstr(const char *s)
   {
      m_tokens.emplace_back(TOK_INSTR, s, 0, ' ');
   }

   void Lexer::addReg(const char *s)
   {
      m_tokens.emplace_back(TOK_REG, s, 0, ' ');
   }

   void Lexer::addNum(int64_t i)
   {
      m_tokens.emplace_back(TOK_NUM, nullptr, i, ' ');
   }

   void Lexer::addPunct(char c)
   {
      m_tokens.emplace_back(TOK_PUNCT, nullptr, 0, c);
   }

   void Lexer::addIdent(const char *s)
   {
      m_tokens.emplace_back(TOK_IDENT, s, 0, ' ');
   }

   void Lexer::resetYasIn()
   {
      fseek(yasin, 0, SEEK_SET);
   }

   void Lexer::reset()
   {
      m_hitError = true;
      m_tokens.clear();
   }

   void Lexer::fail(const char *message)
   {
      if (!m_hitError)
      {
         m_out->out("Error on line %d: %s\n", m_lineno, message);
         m_out->out("Line %d, Byte 0x%.4x: %s\n", m_lineno, m_addr, m_line.c_str());

         fprintf(stderr, "Error on line %d: %s\n", m_lineno, message);
         fprintf(stderr, "Line %d, Byte 0x%.4x: %s\n", m_lineno, m_addr, m_line.c_str());
      }
      m_hitError = true;
   }

   int Lexer::processEmptyLine()
   {
      if (m_tokens.empty())
      {
         if (m_pass > 1)
            dumpLine();
         return DONE;
      }
      // Not an empty line
      return CONTINUE;
   }

   int Lexer::processLabel()
   {
      if (m_tokens.front().type != TOK_IDENT)
         return CONTINUE;

      Token labelToken = m_tokens.front();
      m_tokens.pop_front();
      if (m_tokens.front().type != TOK_PUNCT || m_tokens.front().cval != ':')
      {
         fail("Missing Colon");
         return ERROR;
      }

      if (m_pass == 1)
         m_symbols.emplace(labelToken.sval.c_str(), m_addr);

      m_tokens.pop_front();
      if (m_tokens.empty())
      {
         if (m_pass > 1)
            dumpCode();
         return DONE;
      }

      return CONTINUE;
   }

   int Lexer::processPosInstr()
   {
      if (m_tokens.front().type != TOK_INSTR)
      {
         fail("Bad Instruction");
         return ERROR;
      }

      // It is an instruction but not POS instruction
      if (strcmp(m_tokens.front().sval.c_str(), ".pos") != 0)
         return CONTINUE;

      m_tokens.pop_front();
      if (m_tokens.front().type != TOK_NUM)
      {
         fail("Invalid Address");
         return ERROR;
      }

      m_addr = m_tokens.front().ival;
      if (m_pass > 1)
      {
         dumpCode();
      }
      return DONE;
   }

   int Lexer::processAlignInstr()
   {
      if (m_tokens.front().type != TOK_INSTR)
      {
         fail("Bad Instruction");
         return ERROR;
      }

      // It is an instruction but not ALIGN instruction
      if (strcmp(m_tokens.front().sval.c_str(), ".align") != 0)
         return CONTINUE;

      int a;
      m_tokens.pop_front();
      if (m_tokens.front().type != TOK_NUM || (a = m_tokens.front().ival) <= 0)
      {
         fail("Invalid Alignment");
         return ERROR;
      }

      m_addr = ((m_addr + a - 1) / a) * a;
      if (m_pass > 1)
      {
         dumpCode();
      }
      return DONE;
   }

   int Lexer::processNormalInstr()
   {
      if (m_tokens.front().type != TOK_INSTR)
      {
         fail("Bad Instruction");
         return ERROR;
      }

      auto instr = Instruction(*this);
      return instr.decode(m_tokens.front().sval.c_str());
   }

   int Lexer::findSymbol(const char *name)
   {
      auto iter = m_symbols.find(name);
      if (iter == m_symbols.end())
      {
         std::string msg = "Fail to find label: " + std::string(name);
         fail(msg.c_str());
         return -1;
      }

      return iter->second;
   }

   /**
    * Printing format:
    *                               | <line>
    */
   void Lexer::dumpLine()
   {
      char outstring[33];
      snprintf(outstring, sizeof(outstring), "                            | ");
      m_out->out("%s%s\n", outstring, m_line.c_str());
   }

   /**
    * Printing format:
    *  0xHHHH: cccccccccccccccccccc | <line>
    *      where HHHH is address
    *      cccccccccccccccccccc is code
    */
   void Lexer::dumpCode(const std::vector<char> &decodeBuf)
   {
      if (m_addr > 0xFFFF)
      {
         fail("Code address limit exceeded");
         exit(1);
      }

      char outstring[33];
      if (m_addr > 0xFFF)
      {
         snprintf(outstring, sizeof(outstring), "0x0000:                      | ");
         hexstuff(outstring + 2, m_addr, 4);
         for (size_t i = 0; i < decodeBuf.size(); i++)
            hexstuff(outstring + 7 + 2 * i, decodeBuf[i] & 0xFF, 2);
      }
      else
      {
         snprintf(outstring, sizeof(outstring), "0x000:                      | ");
         hexstuff(outstring + 2, m_addr, 3);
         for (size_t i = 0; i < decodeBuf.size(); i++)
            hexstuff(outstring + 7 + 2 * i, decodeBuf[i] & 0xFF, 2);
      }
      m_out->out("%s%s\n", outstring, m_line.c_str());
   }
}
