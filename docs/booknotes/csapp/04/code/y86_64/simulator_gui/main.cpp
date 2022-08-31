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
   printf("Usage: %s -c <file>.yo -t <file>.tcl\n", pname);
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

   return TCL_OK;
}

struct Options
{
   std::string codeFile;
   std::string tclFile;
};

int main(int argc, char *argv[])
{
   Options option;
   int ch;
   while ((ch = getopt(argc, argv, "hc:t:")) != -1)
   {
      switch (ch)
      {
      case 'h':
         usage(argv[0]);
         return 0;
      case 'c':
         option.codeFile = optarg;
         break;
      case 't':
         option.tclFile = optarg;
         break;
      default:
         break;
         ;
      }
   }

   if (!endsWith(option.codeFile, ".yo") || !endsWith(option.tclFile, ".tcl"))
   {
      usage(argv[0]);
      return -1;
   }

   char *tkargv[2];
   for (size_t i = 0; i < TKARGS; i++)
   {
      if ((tkargv[i] = (char *)malloc(MAXBUF * sizeof(char))) == nullptr)
      {
         perror("malloc error");
         return -1;
      }
   }
   strcpy(tkargv[0], argv[0]);
   strcpy(tkargv[1], option.tclFile.c_str());

   Tk_Main(TKARGS, tkargv, AppInit);
   return 0;
}
