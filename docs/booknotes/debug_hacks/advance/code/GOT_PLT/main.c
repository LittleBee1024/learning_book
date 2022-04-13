#include "./foo.h"

int bar(void)
{
   return 2;
}

int main(void)
{
   int a = bar();
   int b = foo();
   int c = a + b;
   return 0;
}
