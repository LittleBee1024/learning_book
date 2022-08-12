#pragma once

#include "./def.h"

namespace ISA
{
   /**
    * @brief Y86-64 instruction structure
    * 
    */
   struct Instr
   {
      const char *name;
      unsigned char code; /* Byte code for instruction+op */
      int bytes;
      INSTR_ARG arg1;
      int arg1pos;
      int arg1hi; /* 0/1 for register argument, # bytes for allocation */
      INSTR_ARG arg2;
      int arg2pos;
      int arg2hi; /* 0/1 */
   };

   /**
    * @brief Find Y86-64 instruction struction by the instruction name
    * 
    * @param name Instruction name
    * @return const Instr&, return the reference of Y86-64 instruction struction.
    *         If no instruction is found, return the empty instruction.
    */
   const Instr &findInstr(const char *name);

   /**
    * @brief Decode Y86-64 instruction name by the icode
    * 
    * @param code Instruction code number
    * @return const char*, return the instruction name. For invalid icode, return nullptr.
    */
   const char *decodeInstrName(int code);
}
