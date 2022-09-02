#include "./simulator_gui/tcl_cmd.h"
#include "./simulator_gui/c_api_sim.h"
#include "./simulator_gui/sim_render.h"
#include "./simulator_gui/singleton.h"

#include <assert.h>

int simResetCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
   if (argc != 1)
   {
      Tcl_SetResult(interp, (char *)"No arguments allowed", TCL_STATIC);
      return TCL_ERROR;
   }
   sim_reset();
   Tcl_SetResult(interp, (char *)SIM::getStateName(SIM::STAT_OK), TCL_STATIC);
   return TCL_OK;
}

/**
 * argc: 2
 * arg0: command name "simCode"
 * arg1: Y86-64 code filename
 */
int simLoadCodeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
   if (argc != 2)
   {
      Tcl_SetResult(interp, (char *)"One argument for code filename is required", TCL_STATIC);
      return TCL_ERROR;
   }

   const char *codeFile = argv[1];
   assert(codeFile != nullptr);
   sim_load_code(codeFile);

   SIM::SimRender r(interp, SIM::SimSingleton::getInstance());
   r.displayInstr();

   return TCL_OK;
}

/**
 * argc: 2
 * arg0: command name "simRun"
 * arg1: step num
 */
int simRunCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
   if (argc != 2)
   {
      Tcl_SetResult(interp, (char *)"One argument for step number is required", TCL_STATIC);
      return TCL_ERROR;
   }

   word_t step_num = 0;
   if (sscanf(argv[1], "%lld", &step_num) != 1 || step_num < 0)
   {
      char buf[1024];
      sprintf(buf, "Cannot run for '%s' cycles!", argv[1]);
      Tcl_SetResult(interp, buf, TCL_STATIC);
      return TCL_ERROR;
   }

   SIM::SimRender r(interp, SIM::SimSingleton::getInstance());

   r.displayCurPC();
   const char *status = sim_step_run(step_num);
   r.displayStates();

   // Pass simulator status to the TCL result, it has to be at last
   Tcl_SetResult(interp, (char *)status, TCL_STATIC);
   return TCL_OK;
}
