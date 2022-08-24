#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

int RandomObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
   Tcl_Obj *resultPtr;
   int rand, error;
   int range = 0;
   if (objc > 2)
   {
      Tcl_WrongNumArgs(interp, 1, objv, "?range?");
      return TCL_ERROR;
   }
   if (objc == 2)
   {
      if (Tcl_GetIntFromObj(interp, objv[1], &range) !=
          TCL_OK)
      {
         return TCL_ERROR;
      }
   }
   rand = random();
   if (range != 0)
   {
      rand = rand % range;
   }
   resultPtr = Tcl_GetObjResult(interp);
   Tcl_SetIntObj(resultPtr, rand);
   return TCL_OK;
}

int Random_Init(Tcl_Interp *interp)
{
   Tcl_CreateObjCommand(interp, "orandom", RandomObjCmd, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

   // This file is read if no script is supplied in command line
   Tcl_SetVar(interp, "tcl_rcFileName", "cmd.tcl", TCL_GLOBAL_ONLY);

   return TCL_OK;
}

int main(int argc, char *argv[])
{
   Tcl_Main(argc, argv, Random_Init);
   return 0;
}
