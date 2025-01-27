#include "./calc.h"

#include <stdio.h>
#include <unistd.h>

void usage(const char *pname)
{
   printf("Usage: %s [file.txt]\n", pname);
   printf("\tInput arithmetic expressions from stdin or a file\n");
}

int main(int argc, char *argv[])
{
   int ch;
   while ((ch = getopt(argc, argv, "h")) != -1)
   {
      switch (ch)
      {
      case 'h':
         usage(argv[0]);
         return 0;
      }
   }

   const char *filename = (argc == 1) ? nullptr : argv[1];
   Calc calc(filename);
   calc.compute();
   return 0;
}
