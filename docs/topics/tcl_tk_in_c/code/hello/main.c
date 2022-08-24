#include <stdio.h>
#include <tcl.h>

int main(int argc, char *argv[])
{
   Tcl_Interp *interp;
   const char *result;
   int code;

   if (argc != 2)
   {
      fprintf(stderr, "Wrong number of arguments: should be \"%s filename\"\n", argv[0]);
      return -1;
   }

   interp = Tcl_CreateInterp();
   code = Tcl_EvalFile(interp, argv[1]);
   result = Tcl_GetStringResult(interp);
   if (code != TCL_OK)
   {
      printf("Error was: %s\n", result);
      return -1;
   }
   printf("Result was: %s\n", result);
   Tcl_DeleteInterp(interp);
   return 0;
}
