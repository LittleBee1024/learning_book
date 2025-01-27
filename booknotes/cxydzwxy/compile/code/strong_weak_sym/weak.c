#include <stdio.h>

int __attribute__((weak)) bar = 3;

void __attribute__((weak)) foo(int a, int b)
{
   printf("weak version foo(%d, %d) with bar %d, sizeof(bar) = %zu\n", a, b, bar, sizeof(bar));
}
