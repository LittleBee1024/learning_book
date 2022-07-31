#pragma once

#include <stdio.h>

inline void func()
{
   printf("func\n");
}

class Foo
{
public:
   void foo()
   {
      printf("foo\n");
   }
   void bar();
};

extern Foo g_foo;
