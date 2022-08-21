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
            reg.current = reg.next;
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
      fetch.next.reset();
      fetch.op = P_LOAD;
      decode.current.reset();
      decode.next.reset();
      decode.op = P_LOAD;
      execute.current.reset();
      execute.next.reset();
      execute.op = P_LOAD;
      memory.current.reset();
      memory.next.reset();
      memory.op = P_LOAD;
      writeback.current.reset();
      writeback.next.reset();
      writeback.op = P_LOAD;
   }

   bool PipeRegs::hasError() const
   {
      return (fetch.op == P_ERROR || decode.op == P_ERROR || execute.op == P_ERROR ||
              memory.op == P_ERROR || writeback.op == P_ERROR);
   }

}
