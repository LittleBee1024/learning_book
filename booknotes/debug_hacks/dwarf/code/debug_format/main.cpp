#include <stdio.h>

typedef int int_type;
enum myenum { Jan = 1, Feb = 2};

class base
{
public:
   int basemember;
};

class myclass : public base
{
public:
   int a;
   char b[8];
   int_type c;
   myenum myclass_myenum;
   void display(int x)
   {
      x = 4;
   }
};

int main()
{
   int test = 3;
   myclass mc;
   mc.display(test);
}
