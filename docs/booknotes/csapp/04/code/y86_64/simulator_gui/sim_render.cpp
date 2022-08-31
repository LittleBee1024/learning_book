#include "./simulator_gui/sim_render.h"
#include "./simulator_gui/singleton.h"

#include <stdio.h>
#include <assert.h>

namespace SIM
{
   SimRender::SimRender(Tcl_Interp *interp, const SimInterface *sim) : m_interp(interp)
   {
      const SimBase *s = dynamic_cast<const SimBase *>(sim);
      assert(s != nullptr);
      m_sim = s;
   }

   void SimRender::displayInstr() const
   {
      std::string tclCmd;
      char buf[1024];
      for (const auto &code : m_sim->m_code)
      {
         sprintf(buf, "addCodeLine %lld {%s} {%s}\n", code.addr, code.instr.c_str(), code.comment.c_str());
         tclCmd.append(buf);
      }
      int rc = Tcl_Eval(m_interp, tclCmd.c_str());

      if (rc != TCL_OK)
      {
         G_SIM_LOG("[ERROR] Failed to display code: %s\n", Tcl_GetStringResult(m_interp));
      }
   }
}
