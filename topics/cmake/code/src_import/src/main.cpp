#include "./src1/share.h"
#include "./src1/bar.h"
#include "./src2/foo.h"

#include <iostream>

int main()
{
   std::cout << hello("CMake") << std::endl;
   Bar b;
   b.bar();
   Foo f;
   f.foo();
   return 0;
}
