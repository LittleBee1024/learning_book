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

   return TCL_OK;
}

int main(int argc, char *argv[])
{
   Tk_Main(argc, argv, AppInit);
   return 0;
}
