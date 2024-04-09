#include <stdio.h>

void f(void);

int x = 0x1;
int y = 0x2;
int z = 0x3;

int v = 0;

int main()
{
   printf("[main] &x=0x%p, sizeof(x)=%zu\n", &x, sizeof(x));
   printf("[main] &y=0x%p, sizeof(y)=%zu\n", &y, sizeof(y));
   printf("[main] &z=0x%p, sizeof(z)=%zu\n", &z, sizeof(z));
   printf("[main] &v=0x%p, sizeof(v)=%zu\n", &v, sizeof(v));
   printf("[main] before f(), x = 0x%x, y = 0x%x, z = 0x%x\n", x, y, z);
   f();
   printf("[main] after f(), x = 0x%x, y = 0x%x, z = 0x%x\n", x, y, z);
   return 0;
}
