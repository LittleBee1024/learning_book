#pragma once

#include "./simulator/sim_interface.h"
#include "./simulator/seq.h"
#include "./simulator/state.h"

#include "./_common/io_interface.h"

namespace SIM
{
   // Seq simulator is the implementation of SEQ model in the book
   class Seq : public SimBase
   {
   public:
      explicit Seq(std::shared_ptr<IO::OutputInterface> out);
      void reset() override;

   private:
      State runOneCycle() override;

      void fetch();
      void decode();
      void execute();
      void memory();
      void writeBack();
      void updatePC();
   };

}
