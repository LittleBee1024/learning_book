#pragma once

#include "./_common/io_interface.h"
#include "./_common/isa.h"

#include <memory>
#include <vector>

namespace SIM
{
   class Storage
   {
      static constexpr int SIZE_ALIGN = 32;
   public:
      Storage(int len, std::shared_ptr<IO::OutputInterface> out);
      Storage(const Storage &other) = default;
      Storage &operator=(const Storage &) = default;
      virtual ~Storage() = default;

      bool getByte(word_t pos, byte_t *dest) const;
      bool setByte(word_t pos, byte_t val);

      bool getWord(word_t pos, word_t *dest) const;
      bool setWord(word_t pos, word_t val);

      size_t size() const;
      void reset();

   protected:
      std::shared_ptr<IO::OutputInterface> m_out;
      std::vector<byte_t> m_contents;
   };

   class MemStore : public Storage
   {
   public:
      MemStore(int len, std::shared_ptr<IO::OutputInterface> out);

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
      RegStore(int len, std::shared_ptr<IO::OutputInterface> out);

      word_t getRegVal(REG_ID id) const;
      void setRegVal(REG_ID id, word_t val);
   };
}