#pragma once

#include "./state.h"
#include "isa.h"

namespace SIM
{

   struct FetchRegs
   {
      word_t pc = 0;
      State status = STAT_OK;

      void clear() { *this = {}; }
   };

   struct DecodeRegs
   {
      byte_t icode = I_NOP; /* Single byte instruction code */
      byte_t ifun = F_NONE; /* ALU/JMP qualifier */
      byte_t ra = REG_NONE; /* Register ra ID */
      byte_t rb = REG_NONE; /* Register rb ID */
      word_t valc = 0;      /* Instruction word encoding immediate data */
      word_t valp = 0;      /* Incremented program counter */
      State status = STAT_BUBBLE;

      void clear() { *this = {}; }
   };

   struct ExecuteRegs
   {
      byte_t icode = I_NOP;    /* Instruction code */
      byte_t ifun = F_NONE;    /* ALU/JMP qualifier */
      word_t valc = 0;         /* Immediate data */
      word_t vala = 0;         /* valA */
      word_t valb = 0;         /* valB */
      byte_t srca = REG_NONE;  /* Source Reg ID for valA */
      byte_t srcb = REG_NONE;  /* Source Reg ID for valB */
      byte_t deste = REG_NONE; /* Destination register for valE */
      byte_t destm = REG_NONE; /* Destination register for valM */
      State status = STAT_BUBBLE;

      void clear() { *this = {}; }
   };

   struct MemoryRegs
   {
      byte_t icode = I_NOP;    /* Instruction code */
      byte_t ifun = F_NONE;    /* ALU/JMP qualifier */
      bool takebranch = false; /* Taken branch signal */
      word_t vale = 0;         /* valE */
      word_t vala = 0;         /* valA */
      byte_t deste = REG_NONE; /* Destination register for valE */
      byte_t destm = REG_NONE; /* Destination register for valM */
      byte_t srca = REG_NONE;  /* Source register for valA */
      State status = STAT_BUBBLE;

      void clear() { *this = {}; }
   };

   struct WritebackRegs
   {
      byte_t icode = I_NOP;    /* Instruction code */
      byte_t ifun = F_NONE;    /* ALU/JMP qualifier */
      word_t vale = 0;         /* valE */
      word_t valm = 0;         /* valM */
      byte_t deste = REG_NONE; /* Destination register for valE */
      byte_t destm = REG_NONE; /* Destination register for valM */
      State status = STAT_BUBBLE;

      void clear() { *this = {}; }
   };

   enum PipeOp : int
   {
      P_LOAD,
      P_STALL,
      P_BUBBLE,
      P_ERROR
   };

   template <typename PIPE_REG_T>
   struct PipeReg
   {
      PIPE_REG_T current;
      PIPE_REG_T next;
      PipeOp op = P_LOAD;
   };

   struct PipeRegs
   {
      PipeReg<FetchRegs> fetch;
      PipeReg<DecodeRegs> decode;
      PipeReg<ExecuteRegs> execute;
      PipeReg<MemoryRegs> memory;
      PipeReg<WritebackRegs> writeback;

      void update();
      void clear();
      bool hasError() const;
   };

}
