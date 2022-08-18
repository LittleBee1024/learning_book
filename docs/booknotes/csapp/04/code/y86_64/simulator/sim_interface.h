#pragma once

#include "./state.h"

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
   };
}
