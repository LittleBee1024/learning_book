#include "./memlib.h"
#include "./low_api.h"

#include <errno.h>
#include <stdio.h>

#define MAX_HEAP (20*(1<<20))  /* 20 MB */

static char *mem_heap;
static char *mem_brk;
static char *mem_max_addr;

void mem_init(void)
{
   mem_heap = (char *)Malloc(MAX_HEAP);
   mem_brk = (char *)mem_heap;
   mem_max_addr = (char *)(mem_heap + MAX_HEAP);
}

void *mem_sbrk(int incr)
{
   char *old_brk = mem_brk;
   if((incr < 0) || (mem_brk + incr) > mem_max_addr)
   {
      errno = ENOMEM;
      fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory ...\n");
      return (void *)(-1);
   }
   mem_brk += incr;
   return (void *)old_brk;
}

