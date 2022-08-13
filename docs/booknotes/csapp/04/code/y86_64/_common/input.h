#pragma once

#include "./io_interface.h"

#include <string>

namespace CO
{
   class FileIn : public InputInterface
   {
   public:
      explicit FileIn(const char *filename);
      ~FileIn() override;

      FILE *getHandler() override;

   private:
      FILE *m_in;
   };

   class MemIn : public InputInterface
   {
   public:
      explicit MemIn(const char *buf);
      ~MemIn() override;

      FILE *getHandler() override;

   private:
      std::string m_buf;
      FILE *m_in;
   };
}
