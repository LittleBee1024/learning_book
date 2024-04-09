#include <stdio.h>

double x;

void f()
{
   x = -0.0;
   printf("[f] &x=0x%p, sizeof(x)=%zu\n", &x, sizeof(x));
   printf("[f] x = %f\n", x);
}
