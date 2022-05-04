#include "./foo.h"

extern void obj1();
extern void obj2();

int main()
{
   func();
   g_foo.foo();
   obj1();
   obj2();
   return 0;
}
