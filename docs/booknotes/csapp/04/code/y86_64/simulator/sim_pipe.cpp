#include "./sim_pipe.h"

#include "isa.h"

namespace PIPE
{
   byte_t imem_icode = I_NOP;
   byte_t imem_ifun = F_NONE;
   bool imem_error = false;
   bool instr_valid = true;
   word_t d_regvala = 0;
   word_t d_regvalb = 0;
   bool dmem_error = false;
}

namespace SIM
{
   PipeRegs Pipe::s_pipeRegs;

   Pipe::Pipe(IO::OutputInterface &out) : SimBase(out)
   {
      s_pipeRegs.clear();
   }

   State Pipe::run(int maxSteps)
   {
      return STAT_OK;
   }

}
