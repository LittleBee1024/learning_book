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
   // "hello.tcl" has already passed in from the command line
   // Tcl_EvalFile(interp, "hello.tcl");
   return TCL_OK;
}

int main(int argc, char *argv[])
{
   if (argc != 2)
   {
      fprintf(stderr, "Wrong number of arguments: should be \"%s <file>.tcl\"\n", argv[0]);
      return -1;
   }

   Tk_Main(argc, argv, AppInit);
   return 0;
}
