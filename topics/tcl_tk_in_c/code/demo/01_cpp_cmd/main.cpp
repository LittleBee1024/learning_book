#include "./blob.h"

#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

int BlobCmd(ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);
void BlobCleanup(ClientData data);

int Blob_Init(Tcl_Interp *interp)
{
   if (Tcl_Init(interp) == TCL_ERROR)
      return TCL_ERROR;

   Blob *blobPtr = new Blob();
   Tcl_CreateObjCommand(interp, "blob", BlobCmd, (ClientData)blobPtr, BlobCleanup);

   return TCL_OK;
}

/*
 * BlobCmd --
 *
 *      This implements the blob command, which has these
 *      subcommands:
 *          add <name> <age>
 *          find <name>
 *          remove <name>
 *   Results:
 *      A standard Tcl command result.
 */
int BlobCmd(ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
   const char *subCmds[] = {"add", "find", "remove", nullptr};
   enum BlobIx
   {
      AddIx,   // 4 args
      FindIx,  // 3 args
      RemoveIx // 3 args
   };

   if (objc == 1 || objc > 4)
   {
      Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
      return TCL_ERROR;
   }

   int index = 0;
   if (Tcl_GetIndexFromObj(interp, objv[1], subCmds, "option", 0, &index) != TCL_OK)
   {
      return TCL_ERROR;
   }

   if (((index == FindIx || index == RemoveIx) && (objc != 3)) || ((index == AddIx) && (objc != 4)))
   {
      Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
      return TCL_ERROR;
   }

   Blob *blobPtr = (Blob *)data;
   switch (index)
   {
   case AddIx:
   {
      const char *name = Tcl_GetString(objv[2]);
      int age = 0;
      if (Tcl_GetIntFromObj(interp, objv[3], &age) != TCL_OK)
         return TCL_ERROR;
      if (blobPtr->add(name, age))
         return TCL_ERROR;
      return TCL_OK;
   }
   case FindIx:
   {
      const char *name = Tcl_GetString(objv[2]);
      int age = blobPtr->find(name);
      Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
      Tcl_SetIntObj(resultPtr, age);
      return TCL_OK;
   }
   case RemoveIx:
   {
      const char *name = Tcl_GetString(objv[2]);
      blobPtr->remove(name);
      return TCL_OK;
   }
   default:
   {
      Tcl_WrongNumArgs(interp, 1, objv, "wrong sub-cmd");
      return TCL_ERROR;
   }
   }

   return TCL_OK;
}

// Only triggered by Tcl_DeleteCommand(interp, "blob")
// Tcl_Main ends with Tcl_Exit, which doesn't call any delete function but exit the program
void BlobCleanup(ClientData data)
{
   Blob *blobPtr = (Blob *)data;
   delete (blobPtr);
}

int main(int argc, char *argv[])
{
   Tcl_Main(argc, argv, Blob_Init);
   return 0;
}
