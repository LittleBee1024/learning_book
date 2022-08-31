#include "./simulator_gui/tcl_cmd.h"
#include "./simulator_gui/c_api_sim.h"
#include "./simulator_gui/sim_render.h"
#include "./simulator_gui/singleton.h"

#include <assert.h>

int simResetCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
   return TCL_OK;
}

/**
 * argc: 2
 * arg0: command name "simCode"
 * arg1: Y86-64 code filename
 */
int simLoadCodeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
   const char *codeFile = argv[1];
   assert(codeFile != nullptr);
   sim_load_code(codeFile);

   SIM::SimRender r(interp, SIM::SimSingleton::getInstance());
   return r.displayInstr();
}

int simRunCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
   return TCL_OK;
}
