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
            reg.current.clear();
            break;
         }
         case P_LOAD:
         {
            /* copy calculated state from previous stage */
            reg.current = reg.next;
            break;
         }
         case P_ERROR:
         {
            /* Like a bubble, but insert error condition */
            reg.current.clear();
            break;
         }
         case P_STALL:
         default:
            break;
         }
         if (reg.op != P_ERROR)
            reg.op = P_LOAD;
      };

      updateRegs(fetch);
      updateRegs(decode);
      updateRegs(execute);
      updateRegs(memory);
      updateRegs(writeback);
   }

   void PipeRegs::clear()
   {
      fetch.current.clear();
      fetch.next.clear();
      fetch.op = P_LOAD;
      decode.current.clear();
      decode.next.clear();
      decode.op = P_LOAD;
      execute.current.clear();
      execute.next.clear();
      execute.op = P_LOAD;
      memory.current.clear();
      memory.next.clear();
      memory.op = P_LOAD;
      writeback.current.clear();
      writeback.next.clear();
      writeback.op = P_LOAD;
   }

}
