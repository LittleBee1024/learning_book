#include <stdio.h>

int main()
{
   short int v = -12345;
   unsigned short uv = (unsigned short) v;
   printf("v = %d, uv = %u\n", v, uv);

   unsigned short u = 0xFFFF;
   short int tu = (short int) u;
   printf("u = %u, tu = %d\n", u, tu);

   return 0;
}
