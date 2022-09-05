#include "./simulator/storage.h"

#include <assert.h>
#include <string>

namespace
{
   byte_t toNum(char ch, char cl)
   {
      std::string hexNum;
      hexNum.push_back(ch);
      hexNum.push_back(cl);
      return std::stoi(hexNum, nullptr, 16);
   }
}

namespace SIM
{
   Storage::Storage(int len, std::shared_ptr<IO::OutputInterface> out) : m_out(out)
   {
      len = (len + SIZE_ALIGN - 1) / SIZE_ALIGN * SIZE_ALIGN;
      m_contents.resize(len, 0);
   }

   bool Storage::getByte(word_t pos, byte_t *dest) const
   {
      if (pos < 0 || pos >= (word_t)m_contents.size())
         return false;
      *dest = m_contents[pos];
      return true;
   }

   bool Storage::setByte(word_t pos, byte_t val)
   {
      if (pos < 0 || pos >= (word_t)m_contents.size())
         return false;
      m_contents[pos] = val;
      return true;
   }

   bool Storage::getWord(word_t pos, word_t *dest) const
   {
      if (pos < 0 || pos + sizeof(word_t) >= m_contents.size())
         return false;

      word_t val = 0;
      for (size_t i = 0; i < sizeof(word_t); i++)
      {
         // little-endian
         word_t b = m_contents[pos + i] & 0xFF;
         val = val | (b << (sizeof(word_t) * i));
      }
      *dest = val;
      return true;
   }

   bool Storage::setWord(word_t pos, word_t val)
   {
      if (pos < 0 || pos + sizeof(word_t) > m_contents.size())
         return false;
      for (size_t i = 0; i < sizeof(word_t); i++)
      {
         m_contents[pos + i] = (byte_t)val & 0xFF;
         val >>= sizeof(word_t);
      }
      return true;
   }

   size_t Storage::size() const
   {
      return m_contents.size();
   }

   void Storage::reset()
   {
      std::fill(m_contents.begin(), m_contents.end(), 0);
   }

   RegStore::RegStore(int len, std::shared_ptr<IO::OutputInterface> out) : Storage(len, out)
   {
   }

   word_t RegStore::getRegVal(REG_ID id) const
   {
      word_t val = 0;
      if (id >= REG_NONE)
         return 0;
      getWord(id * sizeof(word_t), &val);
      return val;
   }

   void RegStore::setRegVal(REG_ID id, word_t val)
   {
      if (id < REG_NONE)
      {
         setWord(id * sizeof(word_t), val);
      }
   }

   MemStore::MemStore(int len, std::shared_ptr<IO::OutputInterface> out) : Storage(len, out)
   {
   }

   void MemStore::loadOneInstr(int lineno, word_t addr, std::string instr)
   {
      for (size_t i = 0; i < instr.size(); i += 2)
      {
         if (addr >= (word_t)m_contents.size())
         {
            m_out->out("[ERROR] Invalid addr (Line %d): 0x%llx\n", lineno, addr);
            return;
         }
         // little-endian, the least-significant byte at the smallest address
         m_contents[addr++] = toNum(instr[i], instr[i + 1]);
      }
   }
}
