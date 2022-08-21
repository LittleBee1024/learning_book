#pragma once

#include "./sim_interface.h"
#include "./seq.h"
#include "./state.h"

#include "io_interface.h"

namespace SIM
{
   // Seq simulator is the implementation of SEQ model in the book
   class Seq : public SimBase
   {
   public:
      explicit Seq(IO::OutputInterface &out);
      void reset() override;

   private:
      State runOneStep() override;

      void fetch();
      void decode();
      void execute();
      void memory();
      void writeBack();
      void updatePC();
   };

}
