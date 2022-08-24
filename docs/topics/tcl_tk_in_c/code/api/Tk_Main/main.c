#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>
#include <tk.h>

int AppInit(Tcl_Interp *interp)
{
   if (Tcl_Init(interp) == TCL_ERROR)
      return TCL_ERROR;
   if (Tk_Init(interp) == TCL_ERROR)
      return TCL_ERROR;

   // This file is read if no script is supplied in command line
   Tcl_SetVar(interp, "tcl_rcFileName", "hello.tcl", TCL_GLOBAL_ONLY);

   return TCL_OK;
}

int main(int argc, char *argv[])
{
   // comparing to Tcl_Main, Tk_Main create Tk window at the same time
   Tk_Main(argc, argv, AppInit);
   return 0;
}
