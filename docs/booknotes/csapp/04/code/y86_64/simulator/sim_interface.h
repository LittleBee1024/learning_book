#pragma once

#include "./state.h"
#include "isa.h"

namespace SIM
{
   class SimInterface
   {
   public:
      virtual int loadCode(const char *fname) = 0;
      virtual State runOneStep() = 0;

   public:
      SimInterface() = default;
      SimInterface(const SimInterface &) = delete;
      SimInterface &operator=(const SimInterface &) = delete;
      virtual ~SimInterface() = default;

   protected:
      bool checkCond(cc_t cc, COND cType);
      word_t computeALU(ALU op, word_t argA, word_t argB);
      cc_t computeCC(ALU op, word_t argA, word_t argB);
   };
}
