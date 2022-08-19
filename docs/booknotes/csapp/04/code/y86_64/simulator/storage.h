#pragma once

#include "io_interface.h"
#include "isa.h"

#include <vector>

namespace SIM
{
   class Storage
   {
   protected:
      static constexpr int SIZE_ALIGN = 32;
      IO::OutputInterface &m_out;
      std::vector<byte_t> m_contents;

      Storage(int len, IO::OutputInterface &out);
      Storage(const Storage &other);

   public:
      bool getByte(word_t pos, byte_t *dest) const;
      bool setByte(word_t pos, byte_t val);

      bool getWord(word_t pos, word_t *dest) const;
      bool setWord(word_t pos, word_t val);

      size_t size() const;
   };

   class MemStore : public Storage
   {
   public:
      MemStore(int len, IO::OutputInterface &out);
      MemStore(const MemStore &other);

      /**
       * @brief Load Y86_64 machine code from a file
       *
       * @param fname, file name of machine code
       * @return int, the number of bytes has been read
       */
      int load(const char *fname);
   };

   class RegStore : public Storage
   {
   public:
      RegStore(int len, IO::OutputInterface &out);
      RegStore(const RegStore &other);

      word_t getRegVal(REG_ID id) const;
      void setRegVal(REG_ID id, word_t val);
   };
}