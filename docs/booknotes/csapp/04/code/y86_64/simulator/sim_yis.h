#pragma once

#include "./sim_interface.h"
#include "./state.h"

#include "io_interface.h"
#include "isa.h"

namespace SIM
{
   // Yis simulates instruction one by one, doesn't create an universal model for all instructions
   class Yis : public SimBase
   {
   public:
      explicit Yis(std::shared_ptr<IO::OutputInterface> out);
   private:
      State runOneCycle() override;
   };

}
