#include <stdio.h>

#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
int PrintString(const char* str)
{
   printf("[%s]: %s\n", __func__, str);
   return 1;
}

EMSCRIPTEN_KEEPALIVE
const char* GetString()
{
   static const char str[] = "String from C";
   return str;
}

EMSCRIPTEN_KEEPALIVE
int AddInt(int a, int b)
{
   int res = a + b;
   printf("[%s]: %d + %d = %d\n", __func__, a, b, res);
   return res;
}

EMSCRIPTEN_KEEPALIVE
float DivideFloat(float a, float b)
{
   float res = a / b;
   printf("[%s]: %f / %f = %f\n", __func__, a, b, res);
   return res;
}
