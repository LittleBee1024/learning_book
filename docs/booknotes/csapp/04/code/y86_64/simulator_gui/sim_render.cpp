#include "./simulator_gui/sim_render.h"
#include "./simulator_gui/singleton.h"

#include <stdio.h>
#include <assert.h>
#include <sstream>
#include <iomanip>

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
         G_SIM_LOG("[ERROR] Failed to display code: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::displayCurPC() const
   {
      char tclCmd[1024];
      sprintf(tclCmd, "simLabel {%lld} {*}\n", m_sim->m_pc);

      int rc = Tcl_Eval(m_interp, tclCmd);
      if (rc != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to display PC: %s\n", Tcl_GetStringResult(m_interp));

      updatePC();
   }

   void SimRender::displayStages() const
   {
      updateFetchStage();
      updateDecodeStage();
      updateExecuteStage();
      updateMemoryStage();
      updatePCUpdateStage();
   }

   void SimRender::updatePC() const
   {
      std::stringstream tclCmd;
      tclCmd << "updateStage OPC {"
             << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << m_sim->m_pc
             << "}\n";
      int rc = Tcl_Eval(m_interp, tclCmd.str().c_str());
      if (rc != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to update OPC stage: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::updateFetchStage() const
   {
   }

   void SimRender::updateDecodeStage() const
   {
   }

   void SimRender::updateExecuteStage() const
   {
   }

   void SimRender::updateMemoryStage() const
   {
   }

   void SimRender::updatePCUpdateStage() const
   {
      std::stringstream tclCmd;
      tclCmd << "updateStage NPC {"
             << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << m_sim->m_pc
             << "}\n";
      int rc = Tcl_Eval(m_interp, tclCmd.str().c_str());
      if (rc != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to update NPC stage: %s\n", Tcl_GetStringResult(m_interp));
   }

}
