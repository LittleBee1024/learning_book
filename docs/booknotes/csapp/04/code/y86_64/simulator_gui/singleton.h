#pragma once

#include "./simulator/sim_yis.h"
#include "./_common/io_interface.h"

#include <memory>

#define G_SIM_LOG(fmt, ...) ( SIM::simOut->out(fmt, __VA_ARGS__) )

namespace SIM
{
   extern std::shared_ptr<IO::OutputInterface> simOut;

   class YisSingleton
   {
   public:
      static Yis *getInstance();
      //static void out(const char *format, ...);

   private:
      YisSingleton() = delete;
      YisSingleton(const YisSingleton &) = delete;
      void operator=(const YisSingleton &) = delete;
   };
}
