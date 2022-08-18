#pragma once

#include <cstddef>

typedef unsigned char byte_t;
typedef long long int word_t;
typedef long long unsigned uword_t;

/******************
 * Y86_64 Registers
 ******************/
/* REG_NONE is a special one to indicate no register */
enum REG_ID : int
{
   REG_RAX,
   REG_RCX,
   REG_RDX,
   REG_RBX,
   REG_RSP,
   REG_RBP,
   REG_RSI,
   REG_RDI,
   REG_R8,
   REG_R9,
   REG_R10,
   REG_R11,
   REG_R12,
   REG_R13,
   REG_R14,
   REG_NONE = 0xF,
   REG_ERR
};

#define VALID_REG(id) (id >= 0 && id < REG_NONE)

/******************
 * Y86_64 Instructions
 ******************/
/* Different instruction types */
enum INSTR : int
{
   I_HALT,
   I_NOP,
   I_RRMOVQ,
   I_IRMOVQ,
   I_RMMOVQ,
   I_MRMOVQ,
   I_ALU,
   I_JMP,
   I_CALL,
   I_RET,
   I_PUSHQ,
   I_POPQ,
   I_IADDQ,
   I_POP2
};

/* Default function code */
enum I_FUN : int
{
   F_NONE
};

/* Different instruction argument types */
enum INSTR_ARG : int
{
   R_ARG,
   M_ARG,
   I_ARG,
   NO_ARG
};

/* Different ALU operations */
enum ALU : int
{
   A_ADD,
   A_SUB,
   A_AND,
   A_XOR,
   A_NONE
};

/* Different Jump conditions */
enum COND : int
{
   C_YES,
   C_LE,
   C_L,
   C_E,
   C_NE,
   C_GE,
   C_G
};

/* Pack itype and function into single byte */
#define HPACK(hi, lo) ((((hi)&0xF) << 4) | ((lo)&0xF))

/* Unpack byte */
#define HI4(byte) (((byte) >> 4) & 0xF)
#define LO4(byte) ((byte)&0xF)

/* Get the opcode out of one byte instruction field */
#define GET_ICODE(instr) HI4(instr)

/* Get the ALU/JMP function out of one byte instruction field */
#define GET_FUN(instr) LO4(instr)

/******************
 * Y86_64 Condition
 ******************/
typedef unsigned char cc_t;

#define GET_ZF(cc) (((cc) >> 2) & 0x1)
#define GET_SF(cc) (((cc) >> 1) & 0x1)
#define GET_OF(cc) (((cc) >> 0) & 0x1)

#define PACK_CC(z, s, o) (((z) << 2) | ((s) << 1) | ((o) << 0))
#define DEFAULT_CC PACK_CC(1, 0, 0)

namespace ISA
{
   /**
    * @brief Y86-64 instruction structure
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

   /**
    * @brief Find Y86-64 reigster ID by register name
    * 
    * @param name Register name
    * @return Y_REG_ID, for invalid register name, return Y_REG_ID::ERR
    */
   REG_ID findRegister(const char *name);

   /**
    * @brief Get the Y86-64 register name by register ID
    * 
    * @param id Register ID
    * @return const char*, for invalid ID, return nullptr
    */
   const char *getRegName(REG_ID id);
}
