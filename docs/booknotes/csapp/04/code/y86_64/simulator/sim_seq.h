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
      State fetch();
      State decode();
      State execute();
      State memory();
      State writeBack();
      State updatePC();

   private:
      word_t m_ftpc; // fall-through PC
   };

}
