#include <iostream>
#include <stdexcept>

void throwException()
{
   std::cout << "start\n";

   throw std::runtime_error("throw an exception and catched by GDB");

   std::cout << "endl\n";
}

int main()
{
   try
   {
      throwException();
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << '\n';
   }

   return 0;
}
