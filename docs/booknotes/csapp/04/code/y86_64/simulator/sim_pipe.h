#pragma once

#include "./sim_interface.h"
#include "./pipe.h"
#include "./pipe_regs.h"
#include "./state.h"

#include "io_interface.h"

namespace SIM
{
   // Pipe simulator is the implementation of pipeline model in the book
   class Pipe : public SimBase
   {
   public:
      // export pipeline registers as a static varible, so that generated function can use it
      static PipeRegs s_pipeRegs;

   public:
      explicit Pipe(IO::OutputInterface &out);
      void reset() override;

   private:
      State runOneStep() override;
   };
}
