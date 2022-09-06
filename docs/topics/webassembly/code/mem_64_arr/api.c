#include <stdio.h>
#include <stdlib.h>

#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
int64_t* DoubleArr64(const int64_t* buf, const int len) {
   int64_t* ret_buf = (int64_t*)malloc(len * sizeof(int64_t));

   for (int i = 0; i < len; i++) {
      printf("[%s]: Array from JS buf[%d] = %lld(0x%llx)\n", __func__, i, buf[i], buf[i]);
      ret_buf[i] = buf[i] * 2;
   }

   return ret_buf;
}

