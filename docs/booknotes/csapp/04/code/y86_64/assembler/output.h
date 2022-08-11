#pragma once

#include "./yas_interface.h"

#include <fstream>
#include <sstream>
#include <string>

namespace YAS
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
}