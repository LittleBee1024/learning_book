#pragma once

#include "./sim_interface.h"
#include "./seq.h"
#include "./state.h"
#include "./storage.h"

#include "io_interface.h"

namespace SIM
{
   // Seq simulator is the implementation of SEQ+ model in the book
   class Seq : public SimInterface
   {
   public:
      explicit Seq(IO::OutputInterface &out);

      int loadCode(const char *fname) override;
      State runOneStep() override;
      void compare(const SimInterface &other) const override;

   private:
      State updatePC();
      State fetchInstr();

   private:
      IO::OutputInterface &m_out;
      RegStore m_reg;
      MemStore m_mem;
      word_t m_pc;
      word_t m_ftpc; // fall-through PC
      cc_t m_cc;

      struct PCInputs
      {
         byte_t icode = I_NOP;
         word_t valc = 0;
         word_t valm = 0;
         word_t valp = 0;
         bool bcond = false;
      };
      PCInputs m_pcInputs;
   };

}
