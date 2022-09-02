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

   private:
      // A TCL/TK class for GUI
      friend class SimRender;
      // Cache data for GUI
      struct StageData
      {
         struct Fetch
         {
            INSTR icode = I_NOP;
            I_FUN ifun = F_NONE;
            REG_ID rA = REG_NONE;
            REG_ID rB = REG_NONE;
            word_t valC = 0;
            word_t valP = 0;
         } f;
         struct Decode
         {
            word_t valA = 0;
            word_t valB = 0;
            REG_ID dstE = REG_NONE;
            REG_ID dstM = REG_NONE;
            REG_ID srcA = REG_NONE;
            REG_ID srcB = REG_NONE;
         } d;
         struct Execute
         {
            bool cnd = false;
            word_t valE = 0;
         } e;
         struct Memory
         {
            word_t valM = 0;
         } m;
         struct PCUpdate
         {
            word_t newPC = 0;
         } p;

         void reset() { *this = {}; }
      };
      StageData m_stage;
   };

}
