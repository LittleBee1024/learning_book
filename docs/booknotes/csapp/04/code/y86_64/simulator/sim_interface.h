#pragma once

#include "./simulator/state.h"
#include "./simulator/storage.h"

#include "./_common/isa.h"
#include "./_common/io_interface.h"

#include <memory>
#include <vector>

namespace SIM
{
   class SimInterface
   {
   public:
      virtual int loadCode(const char *fname) = 0;
      virtual void takeSnapshot() = 0;
      virtual State run(int maxCycles) = 0;
      virtual void reset() = 0;
      virtual State runOneCycle() = 0;
      virtual void diffReg() const = 0;
      virtual void diffMem() const = 0;

   public:
      SimInterface() = default;
      SimInterface(const SimInterface &) = delete;
      SimInterface &operator=(const SimInterface &) = delete;
      virtual ~SimInterface() = default;
   };

   class SimBase : public SimInterface
   {
   public:
      explicit SimBase(std::shared_ptr<IO::OutputInterface> out);
      int loadCode(const char *fname) override;
      void takeSnapshot() override;
      State run(int maxCycles) override;
      void reset() override;
      void diffReg() const override;
      void diffMem() const override;

   protected:
      bool checkCond(cc_t cc, COND cType);
      word_t computeALU(ALU op, word_t argA, word_t argB);
      cc_t computeCC(ALU op, word_t argA, word_t argB);

   private:
      int parseCode(const char *fname);

   protected:
      static constexpr int REG_SIZE_BYTES = 128;       // 8 bytes * 16 regs
      static constexpr int MEM_SIZE_BYTES = (1 << 13); // 8KB

      std::shared_ptr<IO::OutputInterface> m_out;

      RegStore m_reg;
      MemStore m_mem;
      word_t m_pc;
      cc_t m_cc;
      word_t m_curCyc;

      // snapshot
      RegStore m_regCopy;
      MemStore m_memCopy;

      // parsed code contents
      struct Code
      {
         Code(int l, word_t a, const std::string &i, const std::string &c)
             : lineno(l), addr(a), instr(i), comment(c) {}
         int lineno;
         word_t addr;
         std::string instr;
         std::string comment;
      };
      std::vector<Code> m_code;
   };
}
