#include "./pipe_regs.h"

namespace SIM
{
   void PipeRegs::update()
   {
      auto updateRegs = [](auto &reg)
      {
         switch (reg.op)
         {
         case P_BUBBLE:
         {
            /* insert a bubble into the current stage */
            reg.current.reset();
            break;
         }
         case P_LOAD:
         {
            /* copy calculated state from previous stage */
            reg.current = reg.coming;
            break;
         }
         case P_ERROR:
         {
            /* Like a bubble, but insert error condition */
            reg.current.reset();
            break;
         }
         case P_STALL:
         default:
            break;
         }
         if (reg.op != P_ERROR)
            reg.op = P_LOAD;
         else
            reg.current.status = STAT_ERR_PIPE;
      };

      updateRegs(fetch);
      updateRegs(decode);
      updateRegs(execute);
      updateRegs(memory);
      updateRegs(writeback);
   }

   void PipeRegs::reset()
   {
      fetch.current.reset();
      fetch.coming.reset();
      fetch.op = P_LOAD;
      decode.current.reset();
      decode.coming.reset();
      decode.op = P_LOAD;
      execute.current.reset();
      execute.coming.reset();
      execute.op = P_LOAD;
      memory.current.reset();
      memory.coming.reset();
      memory.op = P_LOAD;
      writeback.current.reset();
      writeback.coming.reset();
      writeback.op = P_LOAD;
   }

}
