#include "./mm.h"

#include <assert.h>
#include <stdio.h>

int main()
{
   int rc = 0;
   rc = mm_init();
   assert(rc == 0);

   void *block_ptr = mm_malloc(1 << 10); // 1KB
   printf("The address from mm_malloc: %p\n", block_ptr);

   void *block_ptr2 = mm_malloc(1 << 10); // 1KB
   printf("The address2 from mm_malloc: %p\n", block_ptr2);
   mm_free(block_ptr);
   mm_free(block_ptr2);

   void *block_ptr3 = mm_malloc(1 << 10); // 1KB
   printf("The address3 from mm_malloc: %p\n", block_ptr3);
   mm_free(block_ptr3);

   return 0;
}
