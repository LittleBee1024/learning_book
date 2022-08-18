#include "./state.h"

namespace SIM
{
   const char *getStateName(State s)
   {
      static const char *stateNames[] = { "OK", "HALT", "ERR_ADR", "ERR_REG", "ERR_INSTR" };
      return stateNames[s];
   }
}
