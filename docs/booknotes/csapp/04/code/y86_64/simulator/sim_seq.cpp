#include "./sim_seq.h"
#include "./seq.h"

#include "isa.h"

namespace SEQ
{
   /*************
    * Fetch stage
    *************/
   // byte0
   byte_t imem_icode = I_NOP;
   byte_t imem_ifun = F_NONE;
   bool imem_error = false;
   byte_t icode = I_NOP;
   word_t ifun = 0;
   bool instr_valid = false;
   // byte1
   word_t ra = REG_NONE;
   word_t rb = REG_NONE;
   // word
   word_t valc = 0;
   // next pc
   word_t valp = 0;

   /*************
    * Decode/WriteBack stage
    *************/
   word_t srcA = REG_NONE;
   word_t srcB = REG_NONE;
   word_t destE = REG_NONE;
   word_t destM = REG_NONE;
   word_t vala = 0;
   word_t valb = 0;

   /*************
    * Execute stage
    *************/
   word_t aluA = 0;
   word_t aluB = 0;
   word_t vale = 0;
   bool cond = false;

   /*************
    * Memory stage
    *************/
   word_t valm = 0;
   bool dmem_error = false;
}

namespace SIM
{

   Seq::Seq(IO::OutputInterface &out) : SimBase(out)
   {
   }

   State Seq::runOneStep()
   {
      fetch();
      decode();
      execute();
      memory();
      writeBack();
      updatePC();
      return (SIM::State)gen_Stat();
   }

   void Seq::fetch()
   {
      // m_pc was updated in last step's updatePC process
      SEQ::valp = m_pc;

      // icode:ifun <- M1[PC]
      byte_t byte0 = 0;
      SEQ::imem_error = !m_mem.getByte(SEQ::valp, &byte0);
      if (SEQ::imem_error)
      {
         m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
         return;
      }
      SEQ::imem_icode = HI4(byte0);
      SEQ::imem_ifun = LO4(byte0);
      SEQ::icode = gen_icode();
      SEQ::ifun = gen_ifun();
      SEQ::instr_valid = gen_instr_valid();
      if (!SEQ::instr_valid)
      {
         m_out.out("[ERROR] PC = 0x%llx, Invalid instruction %.2x\n", m_pc, byte0);
         return;
      }
      SEQ::valp++;

      // rA:rB <- M1[PC+1]
      SEQ::ra = REG_NONE;
      SEQ::rb = REG_NONE;
      if (gen_need_regids())
      {
         byte_t byte1 = 0; // (rA+rB)
         SEQ::imem_error = !m_mem.getByte(SEQ::valp, &byte1);
         if (SEQ::imem_error)
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
            return;
         }
         SEQ::ra = HI4(byte1);
         SEQ::rb = LO4(byte1);
         SEQ::valp++;
      }

      // valC <- M8[PC+2]
      SEQ::valc = 0;
      if (gen_need_valC())
      {
         SEQ::imem_error = !m_mem.getWord(SEQ::valp, &SEQ::valc);
         if (SEQ::imem_error)
         {
            m_out.out("[ERROR] PC = 0x%llx, Invalid instruction address\n", m_pc);
            return;
         }
         SEQ::valp += sizeof(word_t);
      }

      m_out.out("IF: Fetched %s at 0x%llx. ra=%s, rb=%s, valC = 0x%llx\n", ISA::decodeInstrName(HPACK(SEQ::icode, SEQ::ifun)),
                m_pc, ISA::getRegName((REG_ID)SEQ::ra), ISA::getRegName((REG_ID)SEQ::rb), SEQ::valc);

      return;
   }

   void Seq::decode()
   {
      // valA <- R[rA]
      SEQ::srcA = gen_srcA();
      SEQ::vala = 0;
      if (SEQ::srcA != REG_NONE)
         SEQ::vala = m_reg.getRegVal((REG_ID)SEQ::srcA);

      // valB <- R[rB]
      SEQ::srcB = gen_srcB();
      SEQ::valb = 0;
      if (SEQ::srcB != REG_NONE)
         SEQ::valb = m_reg.getRegVal((REG_ID)SEQ::srcB);

      // gen_dstE() depends on cond, so it has been set before gen_dstE()
      SEQ::cond = checkCond(m_cc, (COND)SEQ::ifun);
      SEQ::destE = gen_dstE();
      SEQ::destM = gen_dstM();
   }

   void Seq::execute()
   {
      SEQ::aluA = gen_aluA();
      SEQ::aluB = gen_aluB();
      ALU alufun = (ALU)gen_alufun();
      SEQ::vale = computeALU(alufun, SEQ::aluA, SEQ::aluB);
      if (gen_set_cc())
         m_cc = computeCC(alufun, SEQ::aluA, SEQ::aluB);
   }

   void Seq::memory()
   {
      // valM <- Memory, or valM -> Memory
      word_t mem_addr = gen_mem_addr();
      word_t mem_data = gen_mem_data();
      SEQ::valm = 0;
      if (gen_mem_read())
      {
         SEQ::dmem_error = !m_mem.getWord(mem_addr, &SEQ::valm);
         if (SEQ::dmem_error)
         {
            m_out.out("[ERROR] PC = 0x%llx, Couldn't read at address 0x%llx\n", m_pc, mem_addr);
            return;
         }
      }

      if (gen_mem_write())
      {
         // Do a test read of the data memory to make sure address is OK
         word_t junk;
         SEQ::dmem_error = !m_mem.getWord(mem_addr, &junk);
         if (SEQ::dmem_error)
         {
            m_out.out("[ERROR] PC = 0x%llx, Couldn't write at address 0x%llx\n", m_pc, mem_addr);
            return;
         }

         m_mem.setWord(mem_addr, mem_data);
         m_out.out("IM: PC = 0x%llx, Wrote 0x%llx to address 0x%llx\n", m_pc, mem_data, mem_addr);
      }

      return;
   }

   void Seq::writeBack()
   {
      // Register <- valE, or register <- valM
      if (SEQ::destE != REG_NONE)
         m_reg.setRegVal((REG_ID)SEQ::destE, SEQ::vale);
      if (SEQ::destM != REG_NONE)
         m_reg.setRegVal((REG_ID)SEQ::destM, SEQ::valm);
   }

   void Seq::updatePC()
   {
      // HCL function to generate predicted PC
      m_pc = gen_new_pc();
   }
}
