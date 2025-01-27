#pragma once

#include <tcl.h>
#include <tk.h>

extern "C"
{
   int simResetCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);
   int simLoadCodeCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);
   int simRunCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);
}
