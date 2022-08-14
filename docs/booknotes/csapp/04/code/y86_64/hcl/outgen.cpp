#include "./outgen.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <string>

namespace HCL
{

   OutGen::OutGen(std::unique_ptr<CO::OutputInterface> &&out) : m_out(std::move(out)), m_curLinePos(0), m_indentLevel(0)
   {
   }

   void OutGen::print(const char *fmt, ...)
   {
      static char buf[1024];
      va_list argp;
      va_start(argp, fmt);
      vsprintf(buf, fmt, argp);
      va_end(argp);

      int len = strlen(buf);
      if (m_curLinePos + len > MAX_COLUMN)
      {
         m_out->out("\n");
         int numOfSpace = (m_indentLevel + 1) * INDENT_SPACE;
         std::string blanks(numOfSpace, ' ');
         m_out->out(blanks.c_str());
         m_curLinePos = numOfSpace;
      }
      m_out->out("%s", buf);
      m_curLinePos += len;
   }

   void OutGen::feedLineWithUpIndent()
   {
      m_out->out("\n");
      m_indentLevel++;
      std::string blanks(m_indentLevel * INDENT_SPACE, ' ');
      print(blanks.c_str());
   }

   void OutGen::feedLineWithDownIndent()
   {
      m_out->out("\n");
      m_indentLevel--;
      std::string blanks(m_indentLevel * INDENT_SPACE, ' ');
      print(blanks.c_str());
   }

   void OutGen::terminateLine()
   {
      m_out->out("\n");
      m_curLinePos = 0;
      m_indentLevel = 0;
   }

}
