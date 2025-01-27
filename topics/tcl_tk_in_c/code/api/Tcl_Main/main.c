#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

int Tcl_AppInit(Tcl_Interp *interp)
{
   if (Tcl_Init(interp) == TCL_ERROR)
      return TCL_ERROR;

   // This file is read if no script is supplied in command line
   Tcl_SetVar(interp, "tcl_rcFileName", "hello.tcl", TCL_GLOBAL_ONLY);

   return TCL_OK;
}

int main(int argc, char *argv[])
{
   Tcl_Main(argc, argv, Tcl_AppInit);
   return 0;
}
