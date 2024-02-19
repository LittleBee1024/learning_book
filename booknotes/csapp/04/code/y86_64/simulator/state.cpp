#include "./simulator/state.h"

namespace SIM
{
   const char *getStateName(State s)
   {
      static const char *stateNames[] = {"OK", "HALT", "BUBBLE", "ERR_ADR", "ERR_REG", "ERR_INSTR", "ERR_PIPE"};
      return stateNames[s];
   }
}
