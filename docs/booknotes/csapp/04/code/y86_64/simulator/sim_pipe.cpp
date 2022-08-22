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

   // variables only used in this CPP
   word_t wb_destE = REG_NONE;
   word_t wb_valE = 0;
   word_t wb_destM = REG_NONE;
   word_t wb_valM = 0;
   word_t mem_addr = 0;
   word_t mem_data = 0;
   bool mem_write = false;
   word_t cc_in = DEFAULT_CC;
}

namespace SIM
{
   Pipe::Pipe(IO::OutputInterface &out) : SimBase(out)
   {
   }

   State Pipe::runOneCycle()
   {
      State s = updateSimStates();
      if (s != STAT_OK)
         return s;

      updateCurrentPipeRegs();

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

   // update simulator's states, including PC update, register/memory write
   State Pipe::updateSimStates()
   {
      if (PIPE::wb_destE != REG_NONE)
         m_reg.setRegVal((REG_ID)PIPE::wb_destE, PIPE::wb_valE);
      if (PIPE::wb_destM != REG_NONE)
         m_reg.setRegVal((REG_ID)PIPE::wb_destM, PIPE::wb_valM);
      if (PIPE::mem_write)
      {
         if (!m_mem.setWord(PIPE::mem_addr, PIPE::mem_addr))
         {
            m_out.out("[ERROR] Couldn't write at address 0x%llx\n", PIPE::mem_addr);
            return STAT_ERR_ADDR;
         }
      }
      m_cc = PIPE::cc_in;
      return STAT_OK;
   }

   // update current pipeline registers with the help of comming pipeline registers
   void Pipe::updateCurrentPipeRegs()
   {
      PIPE::pipe_regs.update();
   }

   // update coming decode and fetch pipeline registers
   void Pipe::doFetchStageForComingDecodeAndFetchRegs()
   {
   }

   // update coming writeback pipeline registers, which depends on current memory registers
   void Pipe::doMemoryStageForComingWritebackRegs()
   {
   }

   // update coming memory pipeline registers, which depends on current execute registers
   void Pipe::doExecuteStageForComingMemoryRegs()
   {
   }

   // update coming execute registers, which depends on current decode registers
   void Pipe::doDecodeStageForComingExecuteRegs()
   {
   }

}
