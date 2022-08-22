#include "./sim_pipe.h"
#include "./pipe.h"
#include "./pipe_regs.h"

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
   SIM::PipeRegs pipe_regs;
}

namespace SIM
{
   Pipe::Pipe(IO::OutputInterface &out) : SimBase(out)
   {
   }

   State Pipe::runOneCycle()
   {
      return STAT_OK;
   }

   void Pipe::reset()
   {
      SimBase::reset();
      PIPE::imem_icode = I_NOP;
      PIPE::imem_ifun = F_NONE;
      PIPE::imem_error = false;
      PIPE::instr_valid = true;
      PIPE::d_regvala = 0;
      PIPE::d_regvalb = 0;
      PIPE::dmem_error = false;
      PIPE::pipe_regs.reset();
   }

}
