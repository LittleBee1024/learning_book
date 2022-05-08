#include <stdio.h>
#include <stdlib.h>

u_int64_t max = 0;

int main()
{
   unsigned blocksize[] = { 1024 * 1024, 1024, 1 };
   int i, count;
   for (i = 0; i < 3; i++)
   {
      for (count = 1; ; count++)
      {
         void *block = malloc(max + blocksize[i]);
         if (block)
         {
            max = max + blocksize[i];
            free(block);
         }
         else
         {
            printf("[%d, %d] Failed to malloc size %luB\n", i, count, max + blocksize[i]);
            break;
         }
      }
   }

   printf("maximum malloc size = %luB, %luKB, %luMB, %luGB\n", max, max/1024, max/1024/1024, max/1024/1024/1024);
}
