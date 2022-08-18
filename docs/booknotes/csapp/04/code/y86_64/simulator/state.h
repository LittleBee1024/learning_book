#pragma once

namespace SIM
{
   enum class State
   {
      OK,
      HALT,
      INVALID_ADDR,
      INVALID_REG,
      INVALID_INSTR
   };
}
