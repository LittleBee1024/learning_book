class C
{
public:
   int func(int)
   {
      return 0;
   }

   class C2
   {
   public:
      int func(int)
      {
         return 0;
      }
   };
};

namespace N
{

int func(int)
{
   return 0;
}

int var3 = 3;

class C
{
public:
   int func(int)
   {
      return 0;
   }
};

}

int func(int)
{
   C c;
   c.func(1);
   C::C2 c2;
   c2.func(2);
   N::C c3;
   c3.func(3);
   return 0;
}

float func(float)
{
   return 0.0;
}

int var1 = 1;

extern "C"
{
   int foo(int)
   {
      return 0;
   }

   int var2 = 2;
}
