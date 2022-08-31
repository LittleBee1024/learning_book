#include "./simulator/sim_interface.h"

#include <assert.h>

namespace SIM
{

   SimBase::SimBase(std::shared_ptr<IO::OutputInterface> out) : m_out(out),
                                                                m_reg(REG_SIZE_BYTES, m_out),
                                                                m_mem(MEM_SIZE_BYTES, m_out),
                                                                m_pc(0),
                                                                m_cc(DEFAULT_CC)
   {
   }

   int SimBase::loadCode(const char *fname)
   {
      int bytes = m_mem.load(fname);
      if (bytes)
         m_out->out("[INFO] Loaded %d bytes code\n", bytes);
      return bytes;
   }

   State SimBase::run(int maxCycles)
   {
      SIM::State state = SIM::STAT_OK;
      int i = 0;
      for (i = 0; i < maxCycles && state == SIM::STAT_OK; i++)
      {
         m_out->out("\nCycle %d. CC=%s, Stat=%s\n", i, ISA::getCCName(m_cc), getStateName(state));
         state = runOneCycle();
      }

      m_out->out("\nAfter %d cycles, the state becomes %s\n\n", i, SIM::getStateName(state));
      return state;
   }

   void SimBase::reset()
   {
      m_reg.reset();
      m_mem.reset();
      m_pc = 0;
      m_cc = DEFAULT_CC;
   }

   void SimBase::compare(const SimInterface &other) const
   {
      const SimBase *child = dynamic_cast<const SimBase *>(&other);
      if (!child)
      {
         m_out->out("[ERROR] Compared with an invalid simulator snapshot\n");
         return;
      }

      compareReg(*child);
      compareMem(*child);
   }

   void SimBase::compareReg(const SimBase &other) const
   {
      m_out->out("Changes to registers:\n");

      for (int id = REG_RAX; id < REG_NONE; id++)
      {
         word_t oldVal = other.m_reg.getRegVal((REG_ID)id);
         word_t newVal = m_reg.getRegVal((REG_ID)id);
         if (oldVal != newVal)
         {
            m_out->out("%s:\t0x%.16llx\t0x%.16llx\n", ISA::getRegName((REG_ID)id), oldVal, newVal);
         }
      }
   }

   void SimBase::compareMem(const SimBase &other) const
   {
      m_out->out("Changes to memory:\n");

      assert(m_mem.size() == other.m_mem.size());
      for (size_t i = 0; i < m_mem.size(); i += sizeof(word_t))
      {
         word_t oldVal = 0;
         other.m_mem.getWord(i, &oldVal);
         word_t newVal = 0;
         m_mem.getWord(i, &newVal);
         if (oldVal != newVal)
         {
            m_out->out("0x%.4llx:\t0x%.16llx\t0x%.16llx\n", i, oldVal, newVal);
         }
      }
   }

   bool SimBase::checkCond(cc_t cc, COND cType)
   {
      bool zf = GET_ZF(cc);
      bool sf = GET_SF(cc);
      bool of = GET_OF(cc);
      bool jump = false;

      switch (cType)
      {
      case C_YES:
         jump = true;
         break;
      case C_LE:
         jump = (sf ^ of) | zf;
         break;
      case C_L:
         jump = sf ^ of;
         break;
      case C_E:
         jump = zf;
         break;
      case C_NE:
         jump = zf ^ 1;
         break;
      case C_GE:
         jump = sf ^ of ^ 1;
         break;
      case C_G:
         jump = (sf ^ of ^ 1) & (zf ^ 1);
         break;
      default:
         jump = false;
         break;
      }
      return jump;
   }

   word_t SimBase::computeALU(ALU op, word_t argA, word_t argB)
   {
      word_t val;
      switch (op)
      {
      case A_ADD:
         val = argA + argB;
         break;
      case A_SUB:
         val = argB - argA;
         break;
      case A_AND:
         val = argA & argB;
         break;
      case A_XOR:
         val = argA ^ argB;
         break;
      default:
         val = 0;
      }
      return val;
   }

   cc_t SimBase::computeCC(ALU op, word_t argA, word_t argB)
   {
      word_t val = computeALU(op, argA, argB);
      bool zero = (val == 0);
      bool sign = (val < 0);
      bool ovf;
      switch (op)
      {
      case A_ADD:
         ovf = ((argA < 0) == (argB < 0)) &&
               ((val < 0) != (argA < 0));
         break;
      case A_SUB:
         ovf = ((argA > 0) == (argB < 0)) &&
               ((val < 0) != (argB < 0));
         break;
      case A_AND:
      case A_XOR:
         ovf = false;
         break;
      default:
         ovf = false;
      }
      return PACK_CC(zero, sign, ovf);
   }

};
