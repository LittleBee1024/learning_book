#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

int RandomCmd(ClientData clientData, Tcl_Interp *interp, int argc, CONST char *argv[])
{
   int range = 0;
   char buffer[20];
   if (argc > 2)
   {
      // Compile error: ‘Tcl_Interp’ {aka ‘struct Tcl_Interp’} has no member named ‘result’
      // In tcl8.6, interp->result is not supported by default.
      // To workaround, add compile flag -DUSE_INTERP_RESULT, or use Tcl_SetResult
      // interp->result = "Usage: random ?range?";

      // TCL_STATIC is used in the case where the result is a constant string allocated by the compiler
      // TCL_DYNAMIC is used if the result is allocated with Tcl_Alloc
      // TCL_VOLATILE is used if the result is in a stack variable
      Tcl_SetResult(interp, "Usage: random ?range?", TCL_STATIC);
      return TCL_ERROR;
   }
   if (argc == 2)
   {
      if (Tcl_GetInt(interp, argv[1], &range) != TCL_OK)
      {
         Tcl_SetResult(interp, "Please input a valid number", TCL_STATIC);
         return TCL_ERROR;
      }
   }
   printf("[  C] Process random()\n");
   int rand = random();
   if (range != 0)
   {
      rand = rand % range;
   }
   sprintf(buffer, "%d", rand);
   Tcl_SetResult(interp, buffer, TCL_VOLATILE);
   return TCL_OK;
}

int Random_Init(Tcl_Interp *interp)
{
   Tcl_CreateCommand(interp, "random", RandomCmd, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

   // This file is read if no script is supplied in command line
   Tcl_SetVar(interp, "tcl_rcFileName", "cmd.tcl", TCL_GLOBAL_ONLY);

   return TCL_OK;
}

int main(int argc, char *argv[])
{
   Tcl_Main(argc, argv, Random_Init);
   return 0;
}
