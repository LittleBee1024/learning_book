#include <stdio.h>
#include <tcl.h>

int main(int argc, char *argv[])
{
   if (argc != 2)
   {
      fprintf(stderr, "Wrong number of arguments: should be \"%s <file>.tcl\"\n", argv[0]);
      return -1;
   }

   Tcl_Interp *interp = Tcl_CreateInterp();
   int code = Tcl_EvalFile(interp, argv[1]);
   const char *result = Tcl_GetStringResult(interp);
   if (code != TCL_OK)
   {
      printf("Error was: %s\n", result);
      return -1;
   }
   printf("Result was: %s\n", result);
   Tcl_DeleteInterp(interp);
   return 0;
}
