#pragma once

#include "./simulator/sim_yis.h"

#include <tcl.h>
#include <tk.h>

namespace SIM
{
   class SimRender
   {
   public:
      SimRender(Tcl_Interp *interp, const Yis *sim);
      ~SimRender() = default;
      SimRender(const SimRender&) = delete;
      SimRender &operator=(const SimRender &) = delete;
      void displayInstr() const;
      void displayCurPC() const;
      void displayStages() const;
      void displayCC() const;
      void displayRegisters() const;

   private:
      void updatePC() const;
      void updateFetchStage() const;
      void updateDecodeStage() const;
      void updateExecuteStage() const;
      void updateMemoryStage() const;
      void updatePCUpdateStage() const;

   private:
      Tcl_Interp *m_interp;
      const Yis *m_sim;
   };
}
