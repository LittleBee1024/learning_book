#include "./simulator_gui/sim_render.h"
#include "./simulator_gui/singleton.h"
#include "./_common/isa.h"

#include <stdio.h>
#include <assert.h>
#include <sstream>
#include <iomanip>

namespace SIM
{
   SimRender::SimRender(Tcl_Interp *interp, const Yis *sim) : m_interp(interp), m_sim(sim)
   {
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

      if (Tcl_Eval(m_interp, tclCmd.c_str()) != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to display code: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::displayCurPC() const
   {
      char tclCmd[1024];
      sprintf(tclCmd, "simLabel {%lld} {*}\n", m_sim->m_pc);

      if (Tcl_Eval(m_interp, tclCmd) != TCL_OK)
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

   void SimRender::displayRegisters() const
   {
      std::stringstream tclCmd;
      int id = 0;
      for (id = REG_RAX; id < REG_NONE; id++)
      {
         word_t val = m_sim->m_reg.getRegVal((REG_ID)id);
         tclCmd << "setReg " << id << " " << val << " 0\n";
      }

      if (Tcl_Eval(m_interp, tclCmd.str().c_str()) != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to display Registers: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::displayCC() const
   {
      char tclCmd[1024];
      sprintf(tclCmd, "setCC %d %d %d", GET_ZF(m_sim->m_cc), GET_SF(m_sim->m_cc), GET_OF(m_sim->m_cc));

      if (Tcl_Eval(m_interp, tclCmd) != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to display CC: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::updatePC() const
   {
      std::stringstream tclCmd;
      tclCmd << "updateStage OPC {"
             << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << m_sim->m_pc
             << "}\n";

      if (Tcl_Eval(m_interp, tclCmd.str().c_str()) != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to update OPC stage: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::updateFetchStage() const
   {
      std::stringstream tclCmd;
      tclCmd << "updateStage F {" 
             << ISA::decodeInstrName(HPACK(m_sim->m_stage.f.icode, m_sim->m_stage.f.ifun)) << " "
             << ISA::getRegName(m_sim->m_stage.f.rA) << " "
             << ISA::getRegName(m_sim->m_stage.f.rB) << " "
             << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << m_sim->m_stage.f.valC << " "
             << std::setw(16) << std::setfill('0') << m_sim->m_stage.f.valP << "}\n";

      if (Tcl_Eval(m_interp, tclCmd.str().c_str()) != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to update Fetch stage: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::updateDecodeStage() const
   {
      std::stringstream tclCmd;
      tclCmd << "updateStage D {" 
             << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << m_sim->m_stage.d.valA << " "
             << std::setw(16) << std::setfill('0') << m_sim->m_stage.d.valB << " "
             << ISA::getRegName(m_sim->m_stage.d.dstE) << " "
             << ISA::getRegName(m_sim->m_stage.d.dstM) << " "
             << ISA::getRegName(m_sim->m_stage.d.srcA) << " "
             << ISA::getRegName(m_sim->m_stage.d.srcB) << "}\n";

      if (Tcl_Eval(m_interp, tclCmd.str().c_str()) != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to update Decode stage: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::updateExecuteStage() const
   {
      std::stringstream tclCmd;
      tclCmd << "updateStage E {" << (m_sim->m_stage.e.cnd ? "Y" : "N") << " "
             << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << m_sim->m_stage.e.valE << "}\n";

      if (Tcl_Eval(m_interp, tclCmd.str().c_str()) != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to update Execute stage: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::updateMemoryStage() const
   {
      std::stringstream tclCmd;
      tclCmd << "updateStage M {" 
             << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << m_sim->m_stage.m.valM << "}\n";

      if (Tcl_Eval(m_interp, tclCmd.str().c_str()) != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to update Memory stage: %s\n", Tcl_GetStringResult(m_interp));
   }

   void SimRender::updatePCUpdateStage() const
   {
      std::stringstream tclCmd;
      tclCmd << "updateStage NPC {"
             << std::uppercase << std::hex << std::setw(16) << std::setfill('0') << m_sim->m_stage.p.newPC
             << "}\n";
      int rc = Tcl_Eval(m_interp, tclCmd.str().c_str());
      if (rc != TCL_OK)
         G_SIM_LOG("[ERROR] Failed to update NPC stage: %s\n", Tcl_GetStringResult(m_interp));
   }

}
