#include "./low_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void error(const char *msg)
{
   fprintf(stderr, "%s: %s\n", msg, strerror(errno));
   exit(0);
}

void *Malloc(size_t size)
{
   void *p;

   if ((p = malloc(size)) == NULL)
      error("Malloc error");
   return p;
}
