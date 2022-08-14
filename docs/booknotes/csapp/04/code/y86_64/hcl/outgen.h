#pragma once

#include "io_interface.h"

#include <memory>

namespace HCL
{

   class OutGen
   {
   public:
      explicit OutGen(std::unique_ptr<CO::OutputInterface> &&out);

      void print(const char *fmt, ...);
      void feedLineWithUpIndent();
      void feedLineWithDownIndent();
      void terminateLine();

   private:
      std::unique_ptr<CO::OutputInterface> m_out;

      // format
      static const int MAX_COLUMN = 80;
      static const int INDENT_SPACE = 3;
      int m_curLinePos;
      int m_indentLevel;
   };

}
