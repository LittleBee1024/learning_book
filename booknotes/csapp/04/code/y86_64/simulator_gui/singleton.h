#pragma once

#include "./simulator/sim_yis.h"
#include "./_common/io_interface.h"

#include <memory>

#define G_SIM_LOG(fmt, ...) ( SIM::g_out->out(fmt, __VA_ARGS__) )

namespace SIM
{
   extern std::shared_ptr<IO::OutputInterface> g_out;

   class SimSingleton
   {
   public:
      static Yis *getInstance();

   private:
      SimSingleton() = delete;
      SimSingleton(const SimSingleton &) = delete;
      SimSingleton &operator=(const SimSingleton &) = delete;
   };
}
