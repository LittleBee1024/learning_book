#pragma once

#include "io_interface.h"
#include "./def.h"
#include "./storage.h"

#include <memory>

namespace ISA
{

   enum class StateType
   {
      OK,
      HALT,
      INVALID_ADDR,
      INVALID_REG,
      INVALID_INSTR
   };

   class State
   {
      static constexpr int REG_SIZE_BYTES = 128;       // 8 bytes * 16 regs
      static constexpr int MEM_SIZE_BYTES = (1 << 13); // 8KB
   public:
      explicit State(std::unique_ptr<CO::OutputInterface> &&out);
      int loadCode(const char *fname);
      StateType step();

   private:
      bool checkCond(COND c);
      word_t computeALU(ALU op, word_t argA, word_t argB);
      cc_t computeCC(ALU op, word_t argA, word_t argB);

   private:
      std::unique_ptr<CO::OutputInterface> m_out;

      word_t m_pc;
      RegStore m_reg;
      MemStore m_mem;
      cc_t m_cc;
   };

}
