#pragma once

namespace SIM
{
   enum State : int
   {
      STAT_OK,
      STAT_HLT,
      STAT_ERR_ADDR,
      STAT_ERR_REG,
      STAT_ERR_INSTR
   };

   const char *getStateName(State s);
}
