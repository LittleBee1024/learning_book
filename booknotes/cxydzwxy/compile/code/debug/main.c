#include <stdio.h>

int global_b = 2;

int main()
{
   int a = 1;
   printf("a = %d, global_b = %d\n", a, global_b);
   return 0;
}