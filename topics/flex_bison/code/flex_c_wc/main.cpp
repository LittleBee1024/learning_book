#include "./lexer.h"

#include <stdio.h>
#include <unistd.h>

void usage(const char *pname)
{
   printf("Usage: %s <file>.c/cpp\n", pname);
   printf("\tPlease input c/cpp source code\n");
}

int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      usage(argv[0]);
      return -1;
   }

   const char *filename = argv[1];
   if (access(filename, F_OK) != 0)
   {
      fprintf(stderr, "Cannot access '%s': No such file\n", filename);
      return -1;
   }

   Lexer lex(filename);
   lex.count();
   return 0;
}
