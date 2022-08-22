#pragma once

#include "./sim_interface.h"
#include "./state.h"

#include "io_interface.h"

namespace SIM
{
   // Pipe simulator is the implementation of pipeline model in the book
   class Pipe : public SimBase
   {
   public:
      explicit Pipe(IO::OutputInterface &out);
      void reset() override;

   private:
      State runOneCycle() override;

      // update simulator's states, including PC update, register/memory write
      State updateSimStates();
      // update current pipeline registers
      void updateCurrentPipeRegs();

      /***************************************************************************
       *  Need to do decode after execute & memory stages, and memory stage before
       *  execute, in order to propagate forwarding values properly
       * *************************************************************************/
      // update coming decode and fetch pipeline registers
      void doFetchStageForComingDecodeAndFetchRegs();
      // update coming writeback pipeline registers, which depends on current memory registers
      void doMemoryStageForComingWritebackRegs();
      // update coming memory pipeline registers, which depends on current execute registers
      void doExecuteStageForComingMemoryRegs();
      // update coming execute registers, which depends on current decode registers
      void doDecodeStageForComingExecuteRegs();
   };
}
