#include "./sim_seq.h"
#include "./seq.h"

#include "isa.h"

namespace SEQ
{
   /* Results computed by previous instruction. Used to compute PC in current instruction */
   byte_t prev_icode = I_NOP;
   byte_t prev_ifun = 0;
   word_t prev_valc = 0;
   word_t prev_valm = 0;
   word_t prev_valp = 0;
   bool prev_bcond = false;

   /* Intermdiate stage values that must be used by control functions */
   byte_t imem_icode = I_NOP;
   byte_t imem_ifun = F_NONE;
   byte_t icode = I_NOP;
   word_t ifun = 0;
   word_t ra = REG_NONE;
   word_t rb = REG_NONE;
   word_t valc = 0;
   word_t valp = 0;
   bool imem_error = false;
   bool instr_valid = false;

   word_t vala = 0;
   word_t valb = 0;
   word_t vale = 0;

   bool bcond = false;
   bool cond = false;
   word_t valm = 0;
   bool dmem_error = false;

   byte_t status = SIM::STAT_OK;
}

namespace SIM
{

   Seq::Seq(IO::OutputInterface &out) : m_out(out),
                                        m_reg(REG_SIZE_BYTES, m_out),
                                        m_mem(MEM_SIZE_BYTES, m_out),
                                        m_pc(0),
                                        m_predPC(0),
                                        m_cc(DEFAULT_CC)
   {
   }

   int Seq::loadCode(const char *fname)
   {
      int bytes = m_mem.load(fname);
      if (bytes)
         m_out.out("[INFO] Loaded %d bytes code\n", bytes);
      return bytes;
   }

   State Seq::runOneStep()
   {
      updatePC();

      fetchInstr();

      return STAT_OK;
   }

   void Seq::compare(const SimInterface &other) const
   {
   }

   State Seq::updatePC()
   {
      // update signals for generating PC
      SEQ::prev_icode = m_pcInputs.icode;
      SEQ::prev_valc = m_pcInputs.valc;
      SEQ::prev_valm = m_pcInputs.valm;
      SEQ::prev_valp = m_pcInputs.valp;
      SEQ::prev_bcond = m_pcInputs.bcond;
      // HCL function to generate predicted PC
      m_predPC = gen_pc();
      return STAT_OK;
   }

   State Seq::fetchInstr()
   {
      // icode:ifun <- M1[PC]
      byte_t byte0 = 0;
      if (!m_mem.getByte(m_predPC, &byte0))
      {
         m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_predPC);
         return STAT_ERR_ADDR;
      }
      SEQ::imem_icode = HI4(byte0);
      SEQ::imem_ifun = LO4(byte0);
      SEQ::icode = gen_icode();
      SEQ::ifun = gen_ifun();
      if (!gen_instr_valid())
      {
         m_out.out("[ERROR] PC = 0x%llx, Invalid instruction %.2x\n", m_pc, byte0);
         return STAT_ERR_INSTR;
      }

      return STAT_OK;
   }

}
