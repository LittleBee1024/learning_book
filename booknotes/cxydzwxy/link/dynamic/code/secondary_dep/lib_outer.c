#include "./lib_inner.h"

#include <stdio.h>

static int a;

void bar()
{
   a = 1;
   b = 2;
   printf("bar a = %d, b = %d\n", a, b);
}

void foo()
{
   ext();
   bar();
}
