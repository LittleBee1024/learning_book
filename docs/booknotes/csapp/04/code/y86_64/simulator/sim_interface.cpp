#include "./simulator/sim_interface.h"

#include <assert.h>
#include <algorithm>
#include <fstream>

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

   std::string getBinaryCode(const std::string &line)
   {
      auto end = line.find_first_of('|');
      std::string code = (end == std::string::npos) ? "" : line.substr(0, end);
      return rtrim(ltrim(code));
   }

   std::string getComment(const std::string &line)
   {
      auto start = line.find_first_of('|');
      assert(start != std::string::npos);
      std::string code = rtrim(ltrim(line.substr(start + 1)));
      return code;
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
      auto start = code.find_first_of(':');
      assert(start != std::string::npos);
      std::string instr = ltrim(code.substr(start + 1));
      assert(instr.find(' ') == std::string::npos);
      return instr;
   }
}

namespace SIM
{

   SimBase::SimBase(std::shared_ptr<IO::OutputInterface> out) : m_out(out),
                                                                m_reg(REG_SIZE_BYTES, m_out),
                                                                m_mem(MEM_SIZE_BYTES, m_out),
                                                                m_pc(0),
                                                                m_cc(DEFAULT_CC),
                                                                m_curCyc(0),
                                                                m_regCopy(m_reg),
                                                                m_memCopy(m_mem)
   {
   }

   int SimBase::loadCode(std::shared_ptr<IO::InputInterface> in)
   {
      m_out->out("[INFO] Reset memory before loading code\n");
      m_mem.reset();

      int bytes = parseCode(in);
      for (const auto &code : m_code)
         m_mem.loadOneInstr(code.lineno, code.addr, code.instr);

      m_out->out("[INFO] Loaded %d bytes code to memory\n", bytes);
      return bytes;
   }

   int SimBase::parseCode(std::shared_ptr<IO::InputInterface> in)
   {
      m_code.clear();

      FILE *stream = in->getHandler();
      if (stream == nullptr)
         return 0;

      constexpr int MAX_LINE_LEN = 1024;
      char line[MAX_LINE_LEN];
      int lineno = 0;
      int numBytes = 0;
      while (fgets(line, MAX_LINE_LEN, stream) != nullptr)
      {
         lineno++;
         std::string binCode = getBinaryCode(line);
         std::string comment = getComment(line);
         if (binCode.empty())
            continue;

         if (checkCode(binCode))
         {
            m_out->out("[ERROR] Invalid binary code (Line %d): %s\n", lineno, binCode.c_str());
            return 0;
         }

         word_t addr = getAddr(binCode);
         std::string instr = getInstruction(binCode);
         // has valid machine code
         if (!instr.empty())
         {
            assert(comment.size() > 0);
            constexpr int HEX_LEN_PER_BYTE = 2;
            assert(instr.size() % HEX_LEN_PER_BYTE == 0);
            m_code.emplace_back(lineno, addr, instr, comment);
            numBytes += instr.size() / HEX_LEN_PER_BYTE;
         }
      }

      return numBytes;
   }

   State SimBase::run(int maxCycles)
   {
      SIM::State state = SIM::STAT_OK;
      m_curCyc = 0;
      while (m_curCyc < maxCycles && state == SIM::STAT_OK)
      {
         state = runOneCycle();
         m_out->out("[INFO] Cycle %lld is done with state=%s\n", m_curCyc - 1, SIM::getStateName(state));
      }

      m_out->out("\nAfter %d cycles, the state becomes %s\n\n", m_curCyc, SIM::getStateName(state));
      return state;
   }

   void SimBase::reset()
   {
      m_reg.reset();
      m_mem.reset();
      m_pc = 0;
      m_cc = DEFAULT_CC;
      m_curCyc = 0;
      m_out->out("[INFO] Reset simulator registers and status\n");
   }

   void SimBase::save()
   {
      m_regCopy = m_reg;
      m_memCopy = m_mem;
   }

   void SimBase::recover()
   {
      m_reg = m_regCopy;
      m_mem = m_memCopy;
   }

   void SimBase::diffReg() const
   {
      m_out->out("Changes to registers:\n");

      for (int id = REG_RAX; id < REG_NONE; id++)
      {
         word_t oldVal = m_regCopy.getRegVal((REG_ID)id);
         word_t newVal = m_reg.getRegVal((REG_ID)id);
         if (oldVal != newVal)
         {
            m_out->out("%s:\t0x%.16llx\t0x%.16llx\n", ISA::getRegName((REG_ID)id), oldVal, newVal);
         }
      }
   }

   void SimBase::diffMem() const
   {
      m_out->out("Changes to memory:\n");

      assert(m_mem.size() == m_memCopy.size());
      for (size_t i = 0; i < m_mem.size(); i += sizeof(word_t))
      {
         word_t oldVal = 0;
         m_memCopy.getWord(i, &oldVal);
         word_t newVal = 0;
         m_mem.getWord(i, &newVal);
         if (oldVal != newVal)
         {
            m_out->out("0x%.4llx:\t0x%.16llx\t0x%.16llx\n", i, oldVal, newVal);
         }
      }
   }

   bool SimBase::checkCond(cc_t cc, COND cType)
   {
      bool zf = GET_ZF(cc);
      bool sf = GET_SF(cc);
      bool of = GET_OF(cc);
      bool jump = false;

      switch (cType)
      {
      case C_YES:
         jump = true;
         break;
      case C_LE:
         jump = (sf ^ of) | zf;
         break;
      case C_L:
         jump = sf ^ of;
         break;
      case C_E:
         jump = zf;
         break;
      case C_NE:
         jump = zf ^ 1;
         break;
      case C_GE:
         jump = sf ^ of ^ 1;
         break;
      case C_G:
         jump = (sf ^ of ^ 1) & (zf ^ 1);
         break;
      default:
         jump = false;
         break;
      }
      return jump;
   }

   word_t SimBase::computeALU(ALU op, word_t argA, word_t argB)
   {
      word_t val;
      switch (op)
      {
      case A_ADD:
         val = argA + argB;
         break;
      case A_SUB:
         val = argB - argA;
         break;
      case A_AND:
         val = argA & argB;
         break;
      case A_XOR:
         val = argA ^ argB;
         break;
      default:
         val = 0;
      }
      return val;
   }

   cc_t SimBase::computeCC(ALU op, word_t argA, word_t argB)
   {
      word_t val = computeALU(op, argA, argB);
      bool zero = (val == 0);
      bool sign = (val < 0);
      bool ovf;
      switch (op)
      {
      case A_ADD:
         ovf = ((argA < 0) == (argB < 0)) &&
               ((val < 0) != (argA < 0));
         break;
      case A_SUB:
         ovf = ((argA > 0) == (argB < 0)) &&
               ((val < 0) != (argB < 0));
         break;
      case A_AND:
      case A_XOR:
         ovf = false;
         break;
      default:
         ovf = false;
      }
      return PACK_CC(zero, sign, ovf);
   }

};
