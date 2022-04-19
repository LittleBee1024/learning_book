#include <stdio.h>

extern int b;
extern void ext();
static int a;

void bar()
{
   a = 1;
   printf("bar a = %d, b = %d\n", a, b);
}

void foo()
{
   ext();
   bar();
}

int main()
{
   foo();
   return 0;
}
