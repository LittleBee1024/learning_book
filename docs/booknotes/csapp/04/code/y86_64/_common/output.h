#pragma once

#include "./io_interface.h"

#include <fstream>
#include <sstream>
#include <string>

namespace CO
{
   class FileOut : public OutputInterface
   {
   public:
      explicit FileOut(const char *filename);
      ~FileOut() override;

      void out(const char *format, ...) override;

   private:
      std::ofstream m_out;
   };

   class MemOut : public OutputInterface
   {
   public:
      MemOut() = default;

      void out(const char *format, ...) override;
      std::string dump() const;

   private:
      std::stringstream m_out;
   };

   class StdOut : public OutputInterface
   {
   public:
      StdOut() = default;

      void out(const char *format, ...) override;
   };
}
