#pragma once

#include "./sim_interface.h"
#include "./state.h"
#include "./storage.h"

#include "io_interface.h"
#include "isa.h"

namespace SIM
{
   // YIS simulates instruction one by one, doesn't create an universal model for all instructions
   class YIS : public SimInterface
   {
      static constexpr int REG_SIZE_BYTES = 128;       // 8 bytes * 16 regs
      static constexpr int MEM_SIZE_BYTES = (1 << 13); // 8KB
   public:
      explicit YIS(IO::OutputInterface &out);

      int loadCode(const char *fname) override;
      State runOneStep() override;
      void compare(const YIS &other) const;

   private:
      void compareReg(const YIS &other) const;
      void compareMem(const YIS &other) const;

   private:
      IO::OutputInterface &m_out;

      word_t m_pc;
      RegStore m_reg;
      MemStore m_mem;
      cc_t m_cc;
   };

}
