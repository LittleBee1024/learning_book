#include "./simulator/sim_pipe.h"
#include "./simulator/pipe.h"

#include "./_common/isa.h"

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
   Pipe::Pipe(std::shared_ptr<IO::OutputInterface> out) : SimBase(out)
   {
   }

   State Pipe::runOneCycle()
   {
      updateCurrentPipeRegs();

      doFetchStageForComingDecodeRegs();
      doWritebackStageForComingFetchRegs();
      doMemoryStageForComingWritebackRegs();
      doExecuteStageForComingMemoryRegs();
      doDecodeStageForComingExecuteRegs();

      doStallCheck();

      m_out->out("[INFO] Cycle %lld is done for instruction %lld\n", m_curCyc, m_numInstr);
      if (PIPE::pipe_regs.writeback.current.status != STAT_BUBBLE)
         m_numInstr++;

      return (SIM::State)PIPE::gen_Stat(); // status is set in write back stage
   }

   void Pipe::reset()
   {
      m_numInstr = 0;
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

   void Pipe::updateCurrentPipeRegs()
   {
      PIPE::pipe_regs.update();

      // dump current pipeline registers
      m_out->out("F: predPC = 0x%llx\n", PIPE::pipe_regs.fetch.current.pc);

      m_out->out("D: instr = %s, rA = %s, rB = %s, valC = 0x%llx, valP = 0x%llx, Stat = %s\n",
                ISA::decodeInstrName(HPACK(PIPE::pipe_regs.decode.current.icode, PIPE::pipe_regs.decode.current.ifun)),
                ISA::getRegName((REG_ID)PIPE::pipe_regs.decode.current.ra), ISA::getRegName((REG_ID)PIPE::pipe_regs.decode.current.rb),
                PIPE::pipe_regs.decode.current.valc, PIPE::pipe_regs.decode.current.valp,
                getStateName(PIPE::pipe_regs.decode.current.status));

      m_out->out("E: instr = %s, valC = 0x%llx, valA = 0x%llx, valB = 0x%llx\n   srcA = %s, srcB = %s, dstE = %s, dstM = %s, Stat = %s\n",
                ISA::decodeInstrName(HPACK(PIPE::pipe_regs.execute.current.icode, PIPE::pipe_regs.execute.current.ifun)),
                PIPE::pipe_regs.execute.current.valc, PIPE::pipe_regs.execute.current.vala, PIPE::pipe_regs.execute.current.valb,
                ISA::getRegName((REG_ID)PIPE::pipe_regs.execute.current.srca), ISA::getRegName((REG_ID)PIPE::pipe_regs.execute.current.srcb),
                ISA::getRegName((REG_ID)PIPE::pipe_regs.execute.current.deste), ISA::getRegName((REG_ID)PIPE::pipe_regs.execute.current.destm),
                getStateName(PIPE::pipe_regs.execute.current.status));

      m_out->out("M: instr = %s, Cnd = %d, valE = 0x%llx, valA = 0x%llx\n   dstE = %s, dstM = %s, Stat = %s\n",
                ISA::decodeInstrName(HPACK(PIPE::pipe_regs.memory.current.icode, PIPE::pipe_regs.memory.current.ifun)),
                PIPE::pipe_regs.memory.current.takebranch, PIPE::pipe_regs.memory.current.vale, PIPE::pipe_regs.memory.current.vala,
                ISA::getRegName((REG_ID)PIPE::pipe_regs.memory.current.deste), ISA::getRegName((REG_ID)PIPE::pipe_regs.memory.current.destm),
                getStateName(PIPE::pipe_regs.memory.current.status));

      m_out->out("W: instr = %s, valE = 0x%llx, valM = 0x%llx, dstE = %s, dstM = %s, Stat = %s\n",
                ISA::decodeInstrName(HPACK(PIPE::pipe_regs.writeback.current.icode, PIPE::pipe_regs.writeback.current.ifun)),
                PIPE::pipe_regs.writeback.current.vale, PIPE::pipe_regs.writeback.current.valm,
                ISA::getRegName((REG_ID)PIPE::pipe_regs.writeback.current.deste), ISA::getRegName((REG_ID)PIPE::pipe_regs.writeback.current.destm),
                getStateName(PIPE::pipe_regs.writeback.current.status));
   }

   void Pipe::doFetchStageForComingDecodeRegs()
   {
      m_pc = PIPE::gen_f_pc();
      word_t valp = m_pc;

      byte_t instr = HPACK(I_NOP, F_NONE);
      PIPE::imem_error = !m_mem.getByte(valp, &instr);
      if (PIPE::imem_error)
         m_out->out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
      PIPE::imem_icode = HI4(instr);
      PIPE::imem_ifun = LO4(instr);
      // gen_instr_valid/gen_need_regids/gen_need_valC depends on icode below
      PIPE::pipe_regs.decode.coming.icode = PIPE::gen_f_icode();
      PIPE::pipe_regs.decode.coming.ifun = PIPE::gen_f_ifun();

      PIPE::instr_valid = PIPE::gen_instr_valid();
      if (!PIPE::instr_valid)
         m_out->out("[ERROR] PC = 0x%llx, Invalid instruction %.2x\n", m_pc, instr);
      valp++;

      byte_t regids = HPACK(REG_NONE, REG_NONE);
      if (PIPE::gen_need_regids())
      {
         PIPE::imem_error = !m_mem.getByte(valp, &regids);
         if (PIPE::imem_error)
            m_out->out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
         valp++;
      }
      PIPE::pipe_regs.decode.coming.ra = HI4(regids);
      PIPE::pipe_regs.decode.coming.rb = LO4(regids);

      word_t valc = 0;
      if (PIPE::gen_need_valC())
      {
         PIPE::imem_error = !m_mem.getWord(valp, &valc);
         if (PIPE::imem_error)
            m_out->out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
         valp += sizeof(word_t);
      }
      PIPE::pipe_regs.decode.coming.valc = valc;
      PIPE::pipe_regs.decode.coming.valp = valp;

      PIPE::pipe_regs.decode.coming.status = (SIM::State)PIPE::gen_f_stat();
      PIPE::pipe_regs.decode.coming.stage_pc = m_pc; // for debugging
   }

   void Pipe::doWritebackStageForComingFetchRegs()
   {
      word_t wb_destE = PIPE::gen_w_dstE();
      word_t wb_valE = PIPE::gen_w_valE();
      if (wb_destE != REG_NONE)
         m_reg.setRegVal((REG_ID)wb_destE, wb_valE);

      word_t wb_destM = PIPE::gen_w_dstM();
      word_t wb_valM = PIPE::gen_w_valM();
      if (wb_destM != REG_NONE)
         m_reg.setRegVal((REG_ID)wb_destM, wb_valM);

      PIPE::pipe_regs.fetch.coming.pc = PIPE::gen_f_predPC();
      PIPE::pipe_regs.fetch.coming.status = (PIPE::gen_f_stat() == STAT_OK) ? STAT_OK : STAT_BUBBLE;
   }

   void Pipe::doMemoryStageForComingWritebackRegs()
   {
      PIPE::dmem_error = false;

      bool mem_read = PIPE::gen_mem_read();
      word_t mem_addr = PIPE::gen_mem_addr();
      word_t valm = 0;
      if (mem_read)
      {
         PIPE::dmem_error = PIPE::dmem_error || !m_mem.getWord(mem_addr, &valm);
         if (PIPE::dmem_error)
            m_out->out("[ERROR] Couldn't read at address 0x%llx\n", mem_addr);
      }

      bool mem_write = PIPE::gen_mem_write();
      word_t mem_data = PIPE::pipe_regs.memory.current.vala;
      if (mem_write)
      {
         PIPE::dmem_error = PIPE::dmem_error || !m_mem.setWord(mem_addr, mem_data);
         if (PIPE::dmem_error)
         {
            m_out->out("[ERROR] Couldn't write at address 0x%llx\n", mem_addr);
         }
      }

      PIPE::pipe_regs.writeback.coming.icode = PIPE::pipe_regs.memory.current.icode;
      PIPE::pipe_regs.writeback.coming.ifun = PIPE::pipe_regs.memory.current.ifun;
      PIPE::pipe_regs.writeback.coming.vale = PIPE::pipe_regs.memory.current.vale;
      PIPE::pipe_regs.writeback.coming.valm = valm;
      PIPE::pipe_regs.writeback.coming.deste = PIPE::pipe_regs.memory.current.deste;
      PIPE::pipe_regs.writeback.coming.destm = PIPE::pipe_regs.memory.current.destm;
      PIPE::pipe_regs.writeback.coming.status = (SIM::State)PIPE::gen_m_stat();
      PIPE::pipe_regs.writeback.coming.stage_pc = PIPE::pipe_regs.memory.current.stage_pc;
   }

   void Pipe::doExecuteStageForComingMemoryRegs()
   {
      word_t alufun = PIPE::gen_alufun();
      bool setcc = PIPE::gen_set_cc();
      word_t alua = PIPE::gen_aluA();
      word_t alub = PIPE::gen_aluB();

      PIPE::pipe_regs.memory.coming.takebranch = checkCond(m_cc, (COND)PIPE::pipe_regs.execute.current.ifun);
      PIPE::pipe_regs.memory.coming.vale = computeALU((ALU)alufun, alua, alub);
      if (setcc)
         m_cc = computeCC((ALU)alufun, alua, alub);

      PIPE::pipe_regs.memory.coming.icode = PIPE::pipe_regs.execute.current.icode;
      PIPE::pipe_regs.memory.coming.ifun = PIPE::pipe_regs.execute.current.ifun;
      PIPE::pipe_regs.memory.coming.vala = PIPE::gen_e_valA();
      PIPE::pipe_regs.memory.coming.deste = PIPE::gen_e_dstE();
      PIPE::pipe_regs.memory.coming.destm = PIPE::pipe_regs.execute.current.destm;
      PIPE::pipe_regs.memory.coming.srca = PIPE::pipe_regs.execute.current.srca;
      PIPE::pipe_regs.memory.coming.status = PIPE::pipe_regs.execute.current.status;
      PIPE::pipe_regs.memory.coming.stage_pc = PIPE::pipe_regs.execute.current.stage_pc;
   }

   void Pipe::doDecodeStageForComingExecuteRegs()
   {
      PIPE::pipe_regs.execute.coming.srca = PIPE::gen_d_srcA();
      PIPE::pipe_regs.execute.coming.srcb = PIPE::gen_d_srcB();
      PIPE::pipe_regs.execute.coming.deste = PIPE::gen_d_dstE();
      PIPE::pipe_regs.execute.coming.destm = PIPE::gen_d_dstM();

      PIPE::d_regvala = m_reg.getRegVal((REG_ID)PIPE::pipe_regs.execute.coming.srca);
      PIPE::d_regvalb = m_reg.getRegVal((REG_ID)PIPE::pipe_regs.execute.coming.srcb);

      PIPE::pipe_regs.execute.coming.vala = PIPE::gen_d_valA();
      PIPE::pipe_regs.execute.coming.valb = PIPE::gen_d_valB();

      PIPE::pipe_regs.execute.coming.icode = PIPE::pipe_regs.decode.current.icode;
      PIPE::pipe_regs.execute.coming.ifun = PIPE::pipe_regs.decode.current.ifun;
      PIPE::pipe_regs.execute.coming.valc = PIPE::pipe_regs.decode.current.valc;
      PIPE::pipe_regs.execute.coming.status = PIPE::pipe_regs.decode.current.status;
      PIPE::pipe_regs.execute.coming.stage_pc = PIPE::pipe_regs.decode.current.stage_pc;
   }

   PipeOp Pipe::pipeCntl(const char *name, word_t stall, word_t bubble)
   {
      if (stall)
      {
         if (bubble)
         {
            m_out->out("%s: Conflicting control signals for pipe register\n", name);
            return P_ERROR;
         }
         else
            return P_STALL;
      }
      else
      {
         return bubble ? P_BUBBLE : P_LOAD;
      }
   }

   void Pipe::doStallCheck()
   {
      PIPE::pipe_regs.fetch.op = pipeCntl("F", PIPE::gen_F_stall(), PIPE::gen_F_bubble());
      PIPE::pipe_regs.decode.op = pipeCntl("D", PIPE::gen_D_stall(), PIPE::gen_D_bubble());
      PIPE::pipe_regs.execute.op = pipeCntl("E", PIPE::gen_E_stall(), PIPE::gen_E_bubble());
      PIPE::pipe_regs.memory.op = pipeCntl("M", PIPE::gen_M_stall(), PIPE::gen_M_bubble());
      PIPE::pipe_regs.writeback.op = pipeCntl("W", PIPE::gen_W_stall(), PIPE::gen_W_bubble());
   }

}
