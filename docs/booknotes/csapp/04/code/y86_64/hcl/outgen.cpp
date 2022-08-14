#include "./outgen.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <string>

namespace HCL
{

   OutGen::OutGen(std::unique_ptr<CO::OutputInterface> &&out) : m_out(std::move(out)), m_curLinePos(0), m_indent(0)
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
         std::string blanks(m_indent, ' ');
         m_out->out(blanks.c_str());
         m_curLinePos = m_indent;
      }
      m_out->out("%s", buf);
      m_curLinePos += len;
   }

   void OutGen::upIndent()
   {
      m_indent += INDENT_SPACE;
   }

   void OutGen::downIndent()
   {
      m_indent -= INDENT_SPACE;
   }

   void OutGen::terminateLine()
   {
      m_out->out("\n");
      m_curLinePos = 0;
      m_indent = 0;
   }

}
