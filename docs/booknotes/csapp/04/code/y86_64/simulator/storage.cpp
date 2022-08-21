#include "./storage.h"

#include <algorithm>
#include <fstream>
#include <assert.h>

namespace
{
   const std::string WHITE_SPACE = " \n\r\t\f\v";

   std::string ltrim(const std::string &s)
   {
      size_t start = s.find_first_not_of(WHITE_SPACE);
      return (start == std::string::npos) ? "" : s.substr(start);
   }

   std::string rtrim(const std::string &s)
   {
      size_t end = s.find_last_not_of(WHITE_SPACE);
      return (end == std::string::npos) ? "" : s.substr(0, end + 1);
   }

   bool startsWith(const std::string &s, const std::string &start)
   {
      if (&start == &s)
         return true;
      if (start.length() > s.length())
         return false;
      for (size_t i = 0; i < start.length(); ++i)
      {
         if (start[i] != s[i])
            return false;
      }
      return true;
   }

   std::string getCode(const std::string &line)
   {
      auto end = line.find_first_of('|');
      std::string code = (end == std::string::npos) ? "" : line.substr(0, end);
      return rtrim(ltrim(code));
   }

   bool checkCode(const std::string &code)
   {
      if (!startsWith(code, "0x") || !startsWith(code, "0X"))
         return false;

      auto found = code.find(':');
      return found != std::string::npos;
   }

   word_t getAddr(const std::string &code)
   {
      auto end = code.find_first_of(':');
      assert(end != std::string::npos);
      std::string addr = code.substr(0, end);
      return std::stoll(addr, nullptr, 16);
   }

   std::string getInstruction(const std::string &code)
   {
      auto end = code.find_first_of(':');
      assert(end != std::string::npos);
      std::string instr = ltrim(code.substr(end + 1));
      assert(instr.find(' ') == std::string::npos);
      return instr;
   }

   byte_t toNum(char ch, char cl)
   {
      assert(isxdigit(ch));
      assert(isxdigit(cl));

      std::string hexNum;
      hexNum.push_back(ch);
      hexNum.push_back(cl);
      return std::stoi(hexNum, nullptr, 16);
   }
}

namespace SIM
{
   Storage::Storage(int len, IO::OutputInterface &out) : m_out(out)
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

   RegStore::RegStore(int len, IO::OutputInterface& out) : Storage(len, out)
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

   MemStore::MemStore(int len, IO::OutputInterface& out) : Storage(len, out)
   {
   }

   int MemStore::load(const char *fname)
   {
      std::ifstream ifs(fname);
      if (ifs.fail())
      {
         m_out.out("[ERROR] Cannot access '%s': No such file\n", fname);
         return 0;
      }

      std::string line;
      int lineno = 0;
      int numBytes = 0;
      while (std::getline(ifs, line))
      {
         lineno++;
         std::string code = getCode(line);
         if (code.empty())
            continue;

         if (checkCode(code))
         {
            m_out.out("[ERROR] Invalid code (Line %d): %s\n", lineno, code.c_str());
            return 0;
         }

         word_t addr = getAddr(code);
         std::string instr = getInstruction(code);
         assert(instr.size() % 2 == 0);
         for (size_t i = 0; i < instr.size(); i += 2)
         {
            if (addr >= (word_t)m_contents.size())
            {
               m_out.out("[ERROR] Invalid addr (Line %d): 0x%llx\n", lineno, addr);
               return 0;
            }
            // little-endian, the least-significant byte at the smallest address
            m_contents[addr++] = toNum(instr[i], instr[i + 1]);
            numBytes++;
         }
      }
      return numBytes;
   }
}
