#pragma once

#include "./simulator/sim_interface.h"
#include "./simulator/state.h"
#include "./_common/io_interface.h"

namespace SIM
{
   // Yis simulates instruction one by one, doesn't create an universal model for all instructions
   class Yis : public SimBase
   {
   public:
      explicit Yis(std::shared_ptr<IO::OutputInterface> out);
      State runOneCycle() override;
   };

}
