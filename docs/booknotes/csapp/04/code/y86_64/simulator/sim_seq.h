#pragma once

#include "./sim_interface.h"
#include "./seq.h"
#include "./state.h"

#include "io_interface.h"

namespace SIM
{
   // Seq simulator is the implementation of SEQ+ model in the book
   class Seq : public SimBase
   {
   public:
      explicit Seq(IO::OutputInterface &out);

      State runOneStep() override;

   private:
      State updatePC();
      State fetchInstr();
      State decode();
      State execute();

   private:
      word_t m_ftpc; // fall-through PC

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
