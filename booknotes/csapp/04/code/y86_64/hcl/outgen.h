#pragma once

#include "./_common/io_interface.h"

#include <memory>

namespace HCL
{

   class OutGen
   {
   public:
      explicit OutGen(std::shared_ptr<IO::OutputInterface> out);

      void print(const char *fmt, ...);
      void feedLineWithUpIndent();
      void feedLineWithDownIndent();
      void terminateLine();

   private:
      std::shared_ptr<IO::OutputInterface> m_out;

      // format
      static const int MAX_COLUMN = 100;
      static const int INDENT_SPACE = 3;
      int m_curLinePos;
      int m_indentLevel;
   };

}
