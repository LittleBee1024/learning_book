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
      return STAT_OK;
   }

   void Seq::compare(const SimInterface &other) const
   {
   }
}
