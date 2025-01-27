#include <stdio.h>

#include <emscripten.h>

int64_t g_int64 = 1LL << 32;

EMSCRIPTEN_KEEPALIVE
int64_t* GetInt64Ptr()
{
   return &g_int64;
}

EMSCRIPTEN_KEEPALIVE
void PrintData() {
   printf("[%s]: g_int64 addr = %p, val = %lld(0x%llx)\n", __func__, &g_int64, g_int64, g_int64);
}
