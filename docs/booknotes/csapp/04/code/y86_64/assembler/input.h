#pragma once

#include "./yas_interface.h"

namespace YAS
{

   class FileIn : public InputInterface
   {
   public:
      explicit FileIn(const char *filename);
      ~FileIn() override;

      FILE *getYasIn() override;

   private:
      FILE *m_in;
   };

   class MemIn : public InputInterface
   {
   public:
      explicit MemIn(const char *buf);
      ~MemIn() override;

      FILE *getYasIn() override;

   private:
      std::string m_buf;
      FILE *m_in;
   };

}
