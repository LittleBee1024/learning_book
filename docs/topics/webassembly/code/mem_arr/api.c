#include <stdio.h>
#include <stdlib.h>

#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
int SumArray(const int* buf, const int len) {
   int total = 0;
   for (int i = 0; i < len; i++){
      //printf("[%s]: Array from JS buf[%d] = %d\n", __func__, i, buf[i]);
      total += buf[i];
   }
   return total;
}

EMSCRIPTEN_KEEPALIVE
int* DoubleArr(const int* buf, const int len) {
   int* ret_buf = (int*)malloc(len * sizeof(int));

   for (int i = 0; i < len; i++) {
      //printf("[%s]: Array from JS buf[%d] = %d\n", __func__, i, buf[i]);
      ret_buf[i] = buf[i] * 2;
   }

   return ret_buf;
}

