#include "./instruction.h"

#include <string.h>
#include <vector>

namespace
{
   const std::vector<ISA::Instr> Y86_64_INSTR = {
       {"nop", HPACK(I_NOP, F_NONE), 1, NO_ARG, 0, 0, NO_ARG, 0, 0},
       {"halt", HPACK(I_HALT, F_NONE), 1, NO_ARG, 0, 0, NO_ARG, 0, 0},
       {"rrmovq", HPACK(I_RRMOVQ, F_NONE), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       /* Conditional move instructions are variants of RRMOVQ */
       {"cmovle", HPACK(I_RRMOVQ, C_LE), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmovl", HPACK(I_RRMOVQ, C_L), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmove", HPACK(I_RRMOVQ, C_E), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmovne", HPACK(I_RRMOVQ, C_NE), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmovge", HPACK(I_RRMOVQ, C_GE), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"cmovg", HPACK(I_RRMOVQ, C_G), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       /* arg1hi indicates number of bytes */
       {"irmovq", HPACK(I_IRMOVQ, F_NONE), 10, I_ARG, 2, 8, R_ARG, 1, 0},
       {"rmmovq", HPACK(I_RMMOVQ, F_NONE), 10, R_ARG, 1, 1, M_ARG, 1, 0},
       {"mrmovq", HPACK(I_MRMOVQ, F_NONE), 10, M_ARG, 1, 0, R_ARG, 1, 1},
       {"addq", HPACK(I_ALU, A_ADD), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"subq", HPACK(I_ALU, A_SUB), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"andq", HPACK(I_ALU, A_AND), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       {"xorq", HPACK(I_ALU, A_XOR), 2, R_ARG, 1, 1, R_ARG, 1, 0},
       /* arg1hi indicates number of bytes */
       {"jmp", HPACK(I_JMP, C_YES), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jle", HPACK(I_JMP, C_LE), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jl", HPACK(I_JMP, C_L), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"je", HPACK(I_JMP, C_E), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jne", HPACK(I_JMP, C_NE), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jge", HPACK(I_JMP, C_GE), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"jg", HPACK(I_JMP, C_G), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"call", HPACK(I_CALL, F_NONE), 9, I_ARG, 1, 8, NO_ARG, 0, 0},
       {"ret", HPACK(I_RET, F_NONE), 1, NO_ARG, 0, 0, NO_ARG, 0, 0},
       {"pushq", HPACK(I_PUSHQ, F_NONE), 2, R_ARG, 1, 1, NO_ARG, 0, 0},
       {"popq", HPACK(I_POPQ, F_NONE), 2, R_ARG, 1, 1, NO_ARG, 0, 0},
       {"iaddq", HPACK(I_IADDQ, F_NONE), 10, I_ARG, 2, 8, R_ARG, 1, 0},
       /* this is just a hack to make the I_POP2 code have an associated name */
       {"pop2", HPACK(I_POP2, F_NONE), 0, NO_ARG, 0, 0, NO_ARG, 0, 0},
       /* For allocation instructions, arg1hi indicates number of bytes */
       {".byte", 0x00, 1, I_ARG, 0, 1, NO_ARG, 0, 0},
       {".word", 0x00, 2, I_ARG, 0, 2, NO_ARG, 0, 0},
       {".long", 0x00, 4, I_ARG, 0, 4, NO_ARG, 0, 0},
       {".quad", 0x00, 8, I_ARG, 0, 8, NO_ARG, 0, 0},
       /* empty instruction */
       {nullptr, 0, 0, NO_ARG, 0, 0, NO_ARG, 0, 0}};
}

namespace ISA
{
   const Instr &findInstr(const char *name)
   {
      for (const auto &instr : Y86_64_INSTR)
      {
         if (strcmp(instr.name, name) == 0)
            return instr;
      }
      return Y86_64_INSTR.back(); // invalid instruction
   }

   // Return name of instruction given its encoding
   const char *decodeInstrName(int code)
   {
      for (const auto &instr : Y86_64_INSTR)
      {
         if (code == instr.code)
            return instr.name;
      }
      return nullptr;
   }
}