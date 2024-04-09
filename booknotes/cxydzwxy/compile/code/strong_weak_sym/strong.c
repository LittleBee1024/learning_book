#include <stdio.h>

long bar = 100;

void foo(int a, int b)
{
   printf("strong version foo(%d, %d) with bar %ld, sizeof(bar) = %zu\n", a, b, bar, sizeof(bar));
}