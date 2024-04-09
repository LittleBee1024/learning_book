#include <inttypes.h>

typedef unsigned __int128 uint128_t;
void multi_64(uint128_t *dest, uint64_t x, uint64_t y)
{
   *dest = x * (uint128_t)y;
}

void multi_32(uint64_t *dest, uint32_t a, uint32_t b)
{
   *dest = a * b;
}
