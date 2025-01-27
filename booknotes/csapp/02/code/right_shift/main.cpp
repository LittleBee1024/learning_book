#include <iostream>
#include <bitset>

template<typename T>
std::string to_bin_str(T x)
{
   std::bitset<sizeof(T) * 8> bin(x);
   return bin.to_string();
}

void logic_right_shift()
{
   printf("%s\n", __func__);
   uint8_t i = 0b10010000;
   uint8_t i2 = i >> 4;
   std::cout << "\ti: 0b" << to_bin_str(i)
      << ", (i >> 4): 0b" << to_bin_str(i2)
      << std::endl;
}

void arithmetic_right_shift()
{
   printf("%s\n", __func__);
   int8_t i = 0b10010000;
   int8_t i2 = i >> 4;
   std::cout << "\ti: 0b" << to_bin_str(i)
      << ", (i >> 4): 0b" << to_bin_str(i2)
      << std::endl;

   int8_t j = 0b01010000;
   int8_t j2 = j >> 4;
   std::cout << "\ti: 0b" << to_bin_str(j)
      << ", (i >> 4): 0b" << to_bin_str(j2)
      << std::endl;
}

int main()
{
   logic_right_shift();
   arithmetic_right_shift();
   return 0;
}
