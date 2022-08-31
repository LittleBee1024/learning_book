#pragma once

#include "./simulator/sim_interface.h"
#include "./_common/io_interface.h"

#include <memory>

#define G_SIM_LOG(fmt, ...) ( SIM::simOut->out(fmt, __VA_ARGS__) )

namespace SIM
{
   extern std::shared_ptr<IO::OutputInterface> simOut;

   class SimSingleton
   {
   public:
      static SimInterface *getInstance();

   private:
      SimSingleton() = delete;
      SimSingleton(const SimSingleton &) = delete;
      void operator=(const SimSingleton &) = delete;
   };
}
