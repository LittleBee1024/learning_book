#include "./yis.h"
#include "isa.h"
#include <assert.h>

namespace SIM
{
   YIS::YIS(IO::OutputInterface &out) : m_out(out),
                                        m_pc(0),
                                        m_reg(REG_SIZE_BYTES, m_out),
                                        m_mem(MEM_SIZE_BYTES, m_out)
   {
   }

   int YIS::loadCode(const char *fname)
   {
      int bytes = m_mem.load(fname);
      if (bytes)
         m_out.out("[INFO] Loaded %d bytes code\n", bytes);
      return bytes;
   }

   State YIS::runOneStep()
   {
      word_t ftpc = m_pc; // fall-through PC
      byte_t byte0 = 0;   // (icode+ifun)
      if (!m_mem.getByte(ftpc, &byte0))
      {
         m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
         return STAT_ERR_ADDR;
      }
      ftpc++;

      INSTR icode = (INSTR)HI4(byte0);
      I_FUN ifun = (I_FUN)LO4(byte0);

      byte_t byte1 = 0; // (rA+rB)
      bool needReg = (icode == I_RRMOVQ || icode == I_ALU || icode == I_PUSHQ ||
                      icode == I_POPQ || icode == I_IRMOVQ || icode == I_RMMOVQ ||
                      icode == I_MRMOVQ || icode == I_IADDQ);
      REG_ID rA = REG_NONE;
      REG_ID rB = REG_NONE;
      if (needReg)
      {
         if (!m_mem.getByte(ftpc, &byte1))
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
            return STAT_ERR_ADDR;
         }
         ftpc++;

         if (icode == I_RRMOVQ || icode == I_ALU || icode == I_RMMOVQ || icode == I_MRMOVQ)
         {
            rA = (REG_ID)HI4(byte1);
            rB = (REG_ID)LO4(byte1);
            if (!VALID_REG(rA) || !VALID_REG(rB))
            {
               m_out.out("[ERROR] PC = 0x%llx, Invalid registers rA=0x%.1x, rB=0x%.1x\n", m_pc, rA, rB);
               return STAT_ERR_REG;
            }
         }

         if (icode == I_PUSHQ || icode == I_POPQ)
         {
            rA = (REG_ID)HI4(byte1);
            if (!VALID_REG(rA))
            {
               m_out.out("[ERROR] PC = 0x%llx, Invalid register rA=0x%.1x\n", m_pc, rA);
               return STAT_ERR_REG;
            }
         }

         if (icode == I_IRMOVQ || icode == I_IADDQ)
         {
            rB = (REG_ID)LO4(byte1);
            if (!VALID_REG(rB))
            {
               m_out.out("[ERROR] PC = 0x%llx, Invalid register rB=0x%.1x\n", m_pc, rB);
               return STAT_ERR_REG;
            }
         }
      }

      word_t cval = 0;
      bool needImm = (icode == I_IRMOVQ || icode == I_RMMOVQ || icode == I_MRMOVQ ||
                      icode == I_JMP || icode == I_CALL || icode == I_IADDQ);
      if (needImm)
      {
         if (!m_mem.getWord(ftpc, &cval))
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
            return STAT_ERR_ADDR;
         }
         ftpc += sizeof(word_t);
      }

      switch (icode)
      {
      case I_HALT:
         return STAT_HLT;
      case I_NOP:
         m_pc = ftpc;
         break;
      case I_RRMOVQ: // rrmovq rA, rB || cmovXX rA, rB
      {
         word_t val = m_reg.getRegVal(rA);
         if (checkCond(m_cc, (COND)ifun))
            m_reg.setRegVal(rB, val);
         m_pc = ftpc;
         break;
      }
      case I_IRMOVQ: // irmovq V, rB
      {
         m_reg.setRegVal(rB, cval);
         m_pc = ftpc;
         break;
      }
      case I_RMMOVQ: // rmmovq rA, D(rB)
      {
         cval += m_reg.getRegVal(rB);
         word_t val = m_reg.getRegVal(rA);
         if (m_mem.setWord(cval, val))
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid RMMOVQ instruction address: 0x%llx\n", m_pc, cval);
            return STAT_ERR_ADDR;
         }
         m_pc = ftpc;
         break;
      }
      case I_MRMOVQ: // mrmovq D(rB), rA
      {
         cval += m_reg.getRegVal(rB);
         word_t val = 0;
         if (!m_mem.getWord(cval, &val))
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid MRMOVQ instruction address: 0x%llx\n", m_pc, cval);
            return STAT_ERR_ADDR;
         }
         m_reg.setRegVal(rA, val);
         m_pc = ftpc;
         break;
      }
      case I_ALU: // OPq rA, rB
      {
         word_t argA = m_reg.getRegVal(rA);
         word_t argB = m_reg.getRegVal(rB);
         word_t val = computeALU((ALU)ifun, argA, argB);
         m_reg.setRegVal(rB, val);
         m_cc = computeCC((ALU)ifun, argA, argB);
         m_pc = ftpc;
         break;
      }
      case I_JMP: // jXX Dest
      {
         if (checkCond(m_cc, (COND)ifun))
            m_pc = cval;
         else
            m_pc = ftpc;
         break;
      }
      case I_CALL: // call Dest
      {
         word_t val = m_reg.getRegVal(REG_RSP) - sizeof(word_t);
         m_reg.setRegVal(REG_RSP, val);
         // push PC to stack
         if (!m_mem.setWord(val, ftpc)) // write address to stack
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid CALL instruction stack address: 0x%llx\n", m_pc, val);
            return STAT_ERR_ADDR;
         }
         m_pc = cval;
         break;
      }
      case I_RET: // ret
      {
         // pop address from stack
         word_t dval = m_reg.getRegVal(REG_RSP);
         word_t val = 0;
         if (!m_mem.getWord(dval, &val)) // read from stack
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid RET instruction stack address: 0x%llx\n", m_pc, dval);
            return STAT_ERR_ADDR;
         }
         m_reg.setRegVal(REG_RSP, dval + sizeof(word_t));
         m_pc = val;
         break;
      }
      case I_PUSHQ: // pushq rA
      {
         word_t val = m_reg.getRegVal(rA);
         word_t dval = m_reg.getRegVal(REG_RSP) - sizeof(word_t);
         m_reg.setRegVal(REG_RSP, dval);
         if (!m_mem.setWord(dval, val))
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid PUSHQ instruction stack address: 0x%llx\n", m_pc, dval);
            return STAT_ERR_ADDR;
         }
         m_pc = ftpc;
         break;
      }
      case I_POPQ: // popq rA
      {
         word_t dval = m_reg.getRegVal(REG_RSP);
         m_reg.setRegVal(REG_RSP, dval + sizeof(word_t));
         word_t val = 0;
         if (!m_mem.getWord(dval, &val))
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid POPQ instruction stack address: 0x%llx\n", m_pc, dval);
            return STAT_ERR_ADDR;
         }
         m_reg.setRegVal(rA, val);
         m_pc = ftpc;
         break;
      }
      case I_IADDQ:
      {
         word_t argB = m_reg.getRegVal(rB);
         word_t val = argB + cval;
         m_reg.setRegVal(rB, val);
         m_cc = computeCC(A_ADD, cval, argB);
         m_pc = ftpc;
         break;
      }
      default:
      {
         m_out.out("[ERROR] PC = 0x%llx, Invalid instruction %.2x\n", m_pc, byte0);
         return STAT_ERR_INSTR;
      }
      }
      return STAT_OK;
   }

   void YIS::compare(const SimInterface &other) const
   {
      const YIS *child = dynamic_cast<const YIS *>(&other);
      if (!child)
      {
         m_out.out("[ERROR] Compared with an invalid simulator snapshot\n");
         return;
      }

      compareReg(*child);
      compareMem(*child);
   }

   void YIS::compareReg(const YIS &other) const
   {
      m_out.out("Changes to registers:\n");

      for (int id = REG_RAX; id < REG_NONE; id++)
      {
         word_t oldVal = other.m_reg.getRegVal((REG_ID)id);
         word_t newVal = m_reg.getRegVal((REG_ID)id);
         if (oldVal != newVal)
         {
            m_out.out("%s:\t0x%.16llx\t0x%.16llx\n", ISA::getRegName((REG_ID)id), oldVal, newVal);
         }
      }
   }

   void YIS::compareMem(const YIS &other) const
   {
      m_out.out("Changes to memory:\n");

      assert(m_mem.size() == other.m_mem.size());
      for (size_t i = 0; i < m_mem.size(); i += sizeof(word_t))
      {
         word_t oldVal = 0;
         other.m_mem.getWord(i, &oldVal);
         word_t newVal = 0;
         m_mem.getWord(i, &newVal);
         if (oldVal != newVal)
         {
            m_out.out("0x%.4llx:\t0x%.16llx\t0x%.16llx\n", i, oldVal, newVal);
         }
      }
   }
}
