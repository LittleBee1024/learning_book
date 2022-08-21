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
      explicit Pipe(IO::OutputInterface &out);
      State run(int maxSteps) override;
   
   private:
      PipeRegs m_pipeRegs;
   };
}
