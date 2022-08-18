#include "./register.h"

#include <string.h>
#include <vector>

namespace
{
   const std::vector<ISA::Register> Y86_64_REG = {
       {"%rax",  REG_RAX},
       {"%rcx",  REG_RCX},
       {"%rdx",  REG_RDX},
       {"%rbx",  REG_RBX},
       {"%rsp",  REG_RSP},
       {"%rbp",  REG_RBP},
       {"%rsi",  REG_RSI},
       {"%rdi",  REG_RDI},
       {"%r8",   REG_R8},
       {"%r9",   REG_R9},
       {"%r10",  REG_R10},
       {"%r11",  REG_R11},
       {"%r12",  REG_R12},
       {"%r13",  REG_R13},
       {"%r14",  REG_R14},
       {nullptr, REG_NONE},
       {nullptr, REG_ERR}};
}

namespace ISA
{
   REG_ID findRegister(const char *name)
   {
      for (const auto &reg : Y86_64_REG)
      {
         if (strcmp(name, reg.name) == 0)
            return reg.id;
      }
      return REG_ERR;
   }

   const char *getRegName(REG_ID id)
   {
      if (id >= REG_RAX && id < REG_NONE)
         return Y86_64_REG[id].name;
      return nullptr;
   }

}
