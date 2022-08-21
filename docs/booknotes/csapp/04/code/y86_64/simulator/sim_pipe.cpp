#include "./sim_pipe.h"

#include "isa.h"

namespace SIM
{

   Pipe::Pipe(IO::OutputInterface &out) : SimBase(out)
   {
   }

   State Pipe::run(int maxSteps)
   {
      return STAT_OK;
   }

}
