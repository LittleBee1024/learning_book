#pragma once

#include "io_interface.h"
#include "isa.h"

#include <vector>

namespace SIM
{
   class Storage
   {
      static constexpr int SIZE_ALIGN = 32;
   public:
      Storage(int len, IO::OutputInterface& out);
      /**
       * @brief Load Y86_64 machine code from a file
       * 
       * @param fname, file name of machine code
       * @return int, the number of bytes has been read
       */
      int load(const char *fname);

      bool getByte(word_t pos, byte_t *dest) const;
      bool setByte(word_t pos, byte_t val);

      bool getWord(word_t pos, word_t *dest) const;
      bool setWord(word_t pos, word_t val);

   private:
      IO::OutputInterface &m_out;
      std::vector<byte_t> m_contents;
   };

   typedef Storage MemStore;

   class RegStore : private Storage
   {
   public:
      RegStore(int len, IO::OutputInterface& out);
      word_t getRegVal(REG_ID id) const;
      void setRegVal(REG_ID id, word_t val);
   };
}