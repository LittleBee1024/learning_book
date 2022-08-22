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
      m_pc = PIPE::gen_f_pc();
      word_t valp = m_pc;

      byte_t instr = HPACK(I_NOP, F_NONE);
      PIPE::imem_error = !m_mem.getByte(valp, &instr);
      if (PIPE::imem_error)
         m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
      PIPE::imem_icode = HI4(instr);
      PIPE::imem_ifun = LO4(instr);
      PIPE::pipe_regs.decode.coming.icode = PIPE::gen_f_icode();
      PIPE::pipe_regs.decode.coming.ifun = PIPE::gen_f_ifun();
      PIPE::instr_valid = PIPE::gen_instr_valid();
      if (!PIPE::instr_valid)
         m_out.out("[ERROR] PC = 0x%llx, Invalid instruction %.2x\n", m_pc, instr);
      valp++;

      byte_t regids = HPACK(REG_NONE, REG_NONE);
      if (PIPE::gen_need_regids())
      {
         PIPE::imem_error = !m_mem.getByte(valp, &regids);
         if (PIPE::imem_error)
            m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
         valp++;
      }
      PIPE::pipe_regs.decode.coming.ra = HI4(regids);
      PIPE::pipe_regs.decode.coming.rb = LO4(regids);

      word_t valc = 0;
      if (PIPE::gen_need_valC())
      {
         PIPE::imem_error = !m_mem.getWord(valp, &valc);
         if (PIPE::imem_error)
            m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
         valp += sizeof(word_t);
      }
      PIPE::pipe_regs.decode.coming.valc = valc;
      PIPE::pipe_regs.decode.coming.valp = valp;
      PIPE::pipe_regs.decode.coming.status = (SIM::State)PIPE::gen_f_stat();

      PIPE::pipe_regs.fetch.coming.pc = PIPE::gen_f_predPC();
      PIPE::pipe_regs.fetch.coming.status = (PIPE::pipe_regs.decode.coming.status == STAT_OK) ? STAT_OK : STAT_BUBBLE;

      PIPE::pipe_regs.decode.coming.stage_pc = m_pc; // for debugging
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
