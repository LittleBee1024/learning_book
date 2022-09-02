#include "./simulator/sim_yis.h"
#include "./_common/isa.h"

namespace SIM
{
   Yis::Yis(std::shared_ptr<IO::OutputInterface> out) : SimBase(out)
   {
   }

   void Yis::reset()
   {
      m_stage.reset();
      SimBase::reset();
   }

   State Yis::runOneCycle()
   {
      m_out->out("\n[INFO] Cycle %d starts with CC='%s'\n", m_curCyc++, ISA::getCCName(m_cc));

      m_stage.reset();

      m_stage.f.valP = m_pc; // fall-through PC
      m_out->out("F: predPC = 0x%llx\n", m_stage.f.valP);

      byte_t byte0 = 0;   // (icode+ifun)
      if (!m_mem.getByte(m_stage.f.valP, &byte0))
      {
         m_out->out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
         return STAT_ERR_ADDR;
      }
      m_stage.f.valP++;

      m_stage.f.icode = (INSTR)HI4(byte0);
      m_stage.f.ifun = (I_FUN)LO4(byte0);

      byte_t byte1 = 0; // (rA+rB)
      bool needReg = (m_stage.f.icode == I_RRMOVQ || m_stage.f.icode == I_ALU || m_stage.f.icode == I_PUSHQ ||
                      m_stage.f.icode == I_POPQ || m_stage.f.icode == I_IRMOVQ || m_stage.f.icode == I_RMMOVQ ||
                      m_stage.f.icode == I_MRMOVQ || m_stage.f.icode == I_IADDQ);
      if (needReg)
      {
         if (!m_mem.getByte(m_stage.f.valP, &byte1))
         {
            m_out->out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
            return STAT_ERR_ADDR;
         }
         m_stage.f.valP++;

         if (m_stage.f.icode == I_RRMOVQ || m_stage.f.icode == I_ALU ||
             m_stage.f.icode == I_RMMOVQ || m_stage.f.icode == I_MRMOVQ)
         {
            m_stage.f.rA = (REG_ID)HI4(byte1);
            m_stage.f.rB = (REG_ID)LO4(byte1);
            if (!VALID_REG(m_stage.f.rA) || !VALID_REG(m_stage.f.rB))
            {
               m_out->out("[ERROR] PC = 0x%llx, Invalid registers rA=0x%.1x, rB=0x%.1x\n", m_pc, m_stage.f.rA, m_stage.f.rB);
               return STAT_ERR_REG;
            }
         }

         if (m_stage.f.icode == I_PUSHQ || m_stage.f.icode == I_POPQ)
         {
            m_stage.f.rA = (REG_ID)HI4(byte1);
            if (!VALID_REG(m_stage.f.rA))
            {
               m_out->out("[ERROR] PC = 0x%llx, Invalid register rA=0x%.1x\n", m_pc, m_stage.f.rA);
               return STAT_ERR_REG;
            }
         }

         if (m_stage.f.icode == I_IRMOVQ || m_stage.f.icode == I_IADDQ)
         {
            m_stage.f.rB = (REG_ID)LO4(byte1);
            if (!VALID_REG(m_stage.f.rB))
            {
               m_out->out("[ERROR] PC = 0x%llx, Invalid register rB=0x%.1x\n", m_pc, m_stage.f.rB);
               return STAT_ERR_REG;
            }
         }
      }

      bool needImm = (m_stage.f.icode == I_IRMOVQ || m_stage.f.icode == I_RMMOVQ || m_stage.f.icode == I_MRMOVQ ||
                      m_stage.f.icode == I_JMP || m_stage.f.icode == I_CALL || m_stage.f.icode == I_IADDQ);
      if (needImm)
      {
         if (!m_mem.getWord(m_stage.f.valP, &m_stage.f.valC))
         {
            m_out->out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
            return STAT_ERR_ADDR;
         }

         m_stage.f.valP += sizeof(word_t);
      }
      m_out->out("D: instr = %s, rA = %s, rB = %s, valC = 0x%llx, valP = 0x%llx\n", ISA::decodeInstrName(HPACK(m_stage.f.icode, m_stage.f.ifun)),
         ISA::getRegName((REG_ID)m_stage.f.rA), ISA::getRegName((REG_ID)m_stage.f.rB), m_stage.f.valC, m_stage.f.valP);

      switch (m_stage.f.icode)
      {
      case I_HALT:
         return STAT_HLT;
      case I_NOP:
      {
         m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      case I_RRMOVQ: // rrmovq rA, rB || cmovXX rA, rB
      {
         m_stage.d.srcA = m_stage.f.rA;
         m_stage.d.valA = m_reg.getRegVal(m_stage.d.srcA);
         m_stage.d.dstE = m_stage.f.rB;

         m_stage.e.valE = m_stage.d.valA;

         if ((m_stage.e.cnd = checkCond(m_cc, (COND)m_stage.f.ifun)) == true)
            m_reg.setRegVal(m_stage.d.dstE, m_stage.e.valE);

         m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      case I_IRMOVQ: // irmovq V, rB
      {
         m_stage.d.dstE = m_stage.f.rB;

         m_stage.e.valE = m_stage.f.valC;

         m_reg.setRegVal(m_stage.d.dstE, m_stage.e.valE);

         m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      case I_RMMOVQ: // rmmovq rA, D(rB)
      {
         m_stage.d.srcA = m_stage.f.rA;
         m_stage.d.srcB = m_stage.f.rB;
         m_stage.d.valA = m_reg.getRegVal(m_stage.d.srcA);
         m_stage.d.valB = m_reg.getRegVal(m_stage.d.srcB);

         m_stage.e.valE = m_stage.d.valB + m_stage.f.valC;

         if (m_mem.setWord(m_stage.e.valE, m_stage.d.valA))
         {
            m_out->out("[ERROR] PC = 0x%llx, Invalid RMMOVQ instruction address: 0x%llx\n", m_pc, m_stage.e.valE);
            return STAT_ERR_ADDR;
         }

         m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      case I_MRMOVQ: // mrmovq D(rB), rA
      {
         m_stage.d.srcB = m_stage.f.rB;
         m_stage.d.valB = m_reg.getRegVal(m_stage.d.srcB);
         m_stage.d.dstM = m_stage.f.rA;

         m_stage.e.valE = m_stage.d.valB + m_stage.f.valC;

         if (!m_mem.getWord(m_stage.e.valE, &m_stage.m.valM))
         {
            m_out->out("[ERROR] PC = 0x%llx, Invalid MRMOVQ instruction address: 0x%llx\n", m_pc, m_stage.e.valE);
            return STAT_ERR_ADDR;
         }

         m_reg.setRegVal(m_stage.d.dstM, m_stage.m.valM);

         m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      case I_ALU: // OPq rA, rB
      {
         m_stage.d.srcA = m_stage.f.rA;
         m_stage.d.srcB = m_stage.f.rB;
         m_stage.d.valA = m_reg.getRegVal(m_stage.d.srcA);
         m_stage.d.valB = m_reg.getRegVal(m_stage.d.srcB);
         m_stage.d.dstE = m_stage.f.rB;

         m_stage.e.valE = computeALU((ALU)m_stage.f.ifun, m_stage.d.valA, m_stage.d.valB);
         m_cc = computeCC((ALU)m_stage.f.ifun, m_stage.d.valA, m_stage.d.valB);

         m_reg.setRegVal(m_stage.d.dstE, m_stage.e.valE);

         m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      case I_JMP: // jXX Dest
      {
         if ((m_stage.e.cnd = checkCond(m_cc, (COND)m_stage.f.ifun)) == true)
            m_stage.p.newPC = m_stage.f.valC;
         else
            m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      case I_CALL: // call Dest
      {
         m_stage.d.srcB = REG_RSP;
         m_stage.d.valB = m_reg.getRegVal(m_stage.d.srcB);
         m_stage.d.dstE = REG_RSP;

         m_stage.e.valE = m_stage.d.valB - sizeof(word_t);

         // push PC to stack
         if (!m_mem.setWord(m_stage.e.valE, m_stage.f.valP)) // write address to stack
         {
            m_out->out("[ERROR] PC = 0x%llx, Invalid instruction stack address: 0x%llx\n", m_pc, m_stage.e.valE);
            return STAT_ERR_ADDR;
         }

         m_reg.setRegVal(m_stage.d.dstE, m_stage.e.valE);

         m_stage.p.newPC = m_stage.f.valC;
         break;
      }
      case I_RET: // ret
      {
         m_stage.d.srcA = REG_RSP;
         m_stage.d.srcB = REG_RSP;
         m_stage.d.valA = m_reg.getRegVal(m_stage.d.srcA);
         m_stage.d.valB = m_reg.getRegVal(m_stage.d.srcB);
         m_stage.d.dstE = REG_RSP;

         m_stage.e.valE = m_stage.d.valB + sizeof(word_t);

         // pop address from stack
         if (!m_mem.getWord(m_stage.d.valA, &m_stage.m.valM)) // read from stack
         {
            m_out->out("[ERROR] PC = 0x%llx, Invalid RET instruction stack address: 0x%llx\n", m_pc, m_stage.d.valA);
            return STAT_ERR_ADDR;
         }

         m_reg.setRegVal(m_stage.d.dstE, m_stage.e.valE);

         m_stage.p.newPC = m_stage.m.valM;
         break;
      }
      case I_PUSHQ: // pushq rA
      {
         m_stage.d.srcA = m_stage.f.rA;
         m_stage.d.srcB = REG_RSP;
         m_stage.d.valA = m_reg.getRegVal(m_stage.d.srcA);
         m_stage.d.valB = m_reg.getRegVal(m_stage.d.srcB);
         m_stage.d.dstE = REG_RSP;

         m_stage.e.valE = m_stage.d.valB - sizeof(word_t);

         if (!m_mem.setWord(m_stage.e.valE, m_stage.d.valA))
         {
            m_out->out("[ERROR] PC = 0x%llx, Invalid PUSHQ instruction stack address: 0x%llx\n", m_pc, m_stage.e.valE);
            return STAT_ERR_ADDR;
         }

         m_reg.setRegVal(m_stage.d.dstE, m_stage.e.valE);

         m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      case I_POPQ: // popq rA
      {
         m_stage.d.srcA = REG_RSP;
         m_stage.d.srcB = REG_RSP;
         m_stage.d.valA = m_reg.getRegVal(m_stage.d.srcA);
         m_stage.d.valB = m_reg.getRegVal(m_stage.d.srcB);
         m_stage.d.dstE = REG_RSP;
         m_stage.d.dstM = m_stage.f.rA;

         m_stage.e.valE = m_stage.d.valB + sizeof(word_t);

         if (!m_mem.getWord(m_stage.d.valA, &m_stage.m.valM))
         {
            m_out->out("[ERROR] PC = 0x%llx, Invalid POPQ instruction stack address: 0x%llx\n", m_pc, m_stage.d.valA);
            return STAT_ERR_ADDR;
         }

         m_reg.setRegVal(m_stage.d.dstE, m_stage.e.valE);
         m_reg.setRegVal(m_stage.d.dstM, m_stage.m.valM);

         m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      case I_IADDQ:
      {
         m_stage.d.srcB = m_stage.f.rB;
         m_stage.d.valB = m_reg.getRegVal(m_stage.d.srcB);
         m_stage.d.dstE = m_stage.f.rB;

         m_stage.e.valE = m_stage.f.valC + m_stage.d.valB;
         m_cc = computeCC(A_ADD, m_stage.f.valC, m_stage.d.valB);

         m_reg.setRegVal(m_stage.d.dstE, m_stage.e.valE);

         m_stage.p.newPC = m_stage.f.valP;
         break;
      }
      default:
      {
         m_out->out("[ERROR] PC = 0x%llx, Invalid instruction %.2x\n", m_pc, byte0);
         return STAT_ERR_INSTR;
      }
      }

      m_pc = m_stage.p.newPC;
      return STAT_OK;
   }
}
