#include <stdio.h>

#include <emscripten.h>

int g_int = 42;
double g_double = 3.1415926;

EMSCRIPTEN_KEEPALIVE
int* GetIntPtr()
{
   return &g_int;
}

EMSCRIPTEN_KEEPALIVE
double* GetDoublePtr() {
   return &g_double;
}

EMSCRIPTEN_KEEPALIVE
void PrintData() {
   printf("[%s]: g_int addr = %p, val = %d\n", __func__, &g_int, g_int);
   printf("[%s]: g_double addr = %p, val = %lf\n", __func__, &g_double, g_double);
}
