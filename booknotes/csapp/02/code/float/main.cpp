#include <iostream>

int main()
{
   // 因舍入运算，丢失了3.14
   printf("((3.14 + 1e20) - 1e20) = %f\n", (3.14 + 1e20) - 1e20);
   printf("(3.14 + (1e20 - 1e20)) = %f\n", 3.14 + (1e20 - 1e20));

   // 向零取舍
   float a = -1.999;
   int b = a;
   double c = 1.999;
   int d = c;
   printf("int(%f)=%d\n", a, b);
   printf("int(%f)=%d\n", c, d);
   return 0;
}
