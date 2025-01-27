#pragma once

#include "./simulator/sim_interface.h"
#include "./simulator/state.h"
#include "./simulator/pipe_regs.h"

#include "./_common/io_interface.h"

namespace SIM
{
   // Pipe simulator is the implementation of pipeline model in the book
   class Pipe : public SimBase
   {
   public:
      explicit Pipe(std::shared_ptr<IO::OutputInterface> out);
      void reset() override;
      State runOneCycle() override;

   private:
      // update current pipeline registers base on its comming pipeline registers
      void updateCurrentPipeRegs();

      /***************************************************************************
       * In order to propagate forwarding values properly, the calling sequence of
       * each stage should be modified in reverse order, to make sure the inputs of
       * a stage is unchanged before the stage is done in one cycle.
       * 1. fetch stage     -> update decode pipeline registers
       * 2. writeback stage -> update fetch pipeline registers
       * 3. memory stage    -> update writeback pipeline registers
       * 4. execute stage   -> update memory pipeline registers
       * 5. decode stage    -> update execute pipeline registers
       * *************************************************************************/
      // update coming decode pipeline registers
      void doFetchStageForComingDecodeRegs();
      // update coming fetch pipeline registers
      void doWritebackStageForComingFetchRegs();
      // update coming writeback pipeline registers
      void doMemoryStageForComingWritebackRegs();
      // update coming memory pipeline registers
      void doExecuteStageForComingMemoryRegs();
      // update coming execute registers
      void doDecodeStageForComingExecuteRegs();

      // update pipeline operations
      void doStallCheck();
      PipeOp pipeCntl(const char *name, word_t stall, word_t bubble);

   private:
      word_t m_numInstr = 0;
   };
}
