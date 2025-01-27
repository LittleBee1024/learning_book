#include "./simulator_gui/tcl_cmd.h"

#include <tcl.h>
#include <tk.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#define MAXBUF 1024
#define TKARGS 3

void usage(char *pname)
{
   printf("Usage: %s -c <file>.yo -g <file>.tcl\n", pname);
   printf("\t<file>.yo  : Y86-64 code file *.yo\n");
   printf("\t<file>.tcl : TCL/TK commands for GUI\n");
}

bool endsWith(const std::string &str, const std::string &suffix)
{
   return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

int AppInit(Tcl_Interp *interp)
{
   if (Tcl_Init(interp) == TCL_ERROR)
      return TCL_ERROR;
   if (Tk_Init(interp) == TCL_ERROR)
      return TCL_ERROR;

   Tcl_CreateCommand(interp, "simReset", (Tcl_CmdProc *)simResetCmd,
                     (ClientData) nullptr, (Tcl_CmdDeleteProc *)nullptr);
   Tcl_CreateCommand(interp, "simCode", (Tcl_CmdProc *)simLoadCodeCmd,
                     (ClientData) nullptr, (Tcl_CmdDeleteProc *)nullptr);
   Tcl_CreateCommand(interp, "simRun", (Tcl_CmdProc *)simRunCmd,
                     (ClientData) nullptr, (Tcl_CmdDeleteProc *)nullptr);

   return TCL_OK;
}

struct Options
{
   std::string codeFile;
   std::string guiFile;
};

int main(int argc, char *argv[])
{
   Options option;
   int ch;
   while ((ch = getopt(argc, argv, "hc:g:")) != -1)
   {
      switch (ch)
      {
      case 'h':
         usage(argv[0]);
         return 0;
      case 'c':
         option.codeFile = optarg;
         break;
      case 'g':
         option.guiFile = optarg;
         break;
      default:
         break;
         ;
      }
   }

   if (!endsWith(option.codeFile, ".yo") || !endsWith(option.guiFile, ".tcl"))
   {
      usage(argv[0]);
      return -1;
   }

   // 0: exec filename
   // 1: gui tcl filename
   // 2: Y86-64 code filename
   // 3: nullptr
   char *tkargv[TKARGS + 1];
   for (size_t i = 0; i < TKARGS; i++)
   {
      if ((tkargv[i] = (char *)malloc(MAXBUF * sizeof(char))) == nullptr)
      {
         perror("malloc error");
         return -1;
      }
   }
   strcpy(tkargv[0], argv[0]);
   strcpy(tkargv[1], option.guiFile.c_str());
   strcpy(tkargv[2], option.codeFile.c_str());
   tkargv[3] = nullptr;

   Tk_Main(TKARGS, tkargv, AppInit);
   return 0;
}
