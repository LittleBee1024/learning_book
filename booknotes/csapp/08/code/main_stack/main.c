#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
   printf("Command line arguments:\n");
   int i = 0;
   while(argv[i] != NULL)
   {
      printf("\targv[%d]: %s\n", i, argv[i]);
      i++;
   }

   printf("Environment variables:\n");
   i = 0;
   while(envp[i] != NULL)
   {
      printf("\tenvp[%d]: %s\n", i, envp[i]);
      i++;
   }
   return 0;
}
