#pragma once

#include "./simulator/sim_interface.h"

#include <tcl.h>
#include <tk.h>

namespace SIM
{
   class SimRender
   {
   public:
      SimRender(Tcl_Interp *interp, const SimInterface *sim);
      ~SimRender() = default;
      SimRender(const SimRender&) = delete;
      SimRender &operator=(const SimRender &) = delete;
      int displayInstr() const;

   private:
      Tcl_Interp *m_interp;
      const SimBase *m_sim;
   };
}
