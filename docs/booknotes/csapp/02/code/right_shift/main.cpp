#include <iostream>
#include <bitset>

void logic_right_shift()
{
   printf("%s\n", __func__);
   uint8_t i = 0b10010000;
   uint8_t i2 = i >> 4;
   std::bitset<8> b(i);
   std::bitset<8> b2(i2);
   std::cout << "\ti: 0b" << b
      << ", (i >> 4): 0b" << b2
      << std::endl;
}

void arithmetic_right_shift()
{
   printf("%s\n", __func__);
   int8_t i = 0b10010000;
   int8_t i2 = i >> 4;
   std::bitset<8> b(i);
   std::bitset<8> b2(i2);
   std::cout << "\ti: 0b" << b
      << ", (i >> 4): 0b" << b2
      << std::endl;
}

int main()
{
   logic_right_shift();
   arithmetic_right_shift();
   return 0;
}
