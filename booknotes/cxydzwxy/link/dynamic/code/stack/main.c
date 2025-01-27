#include <stdio.h>
#include <elf.h>
#include <assert.h>

int main(int argc, char** argv, char** environ)
{
   uint64_t* p = (uint64_t*)argv;
   printf("Argument count: %ld\n", *(p - 1));
   assert(argc == *(p - 1));

   int i = 0;
   for (; i < *(p - 1); i++)
   {
      printf("Argument %d : %s\n", i, (char *)(*(p + i)));
   }

   p += i;
   p++; // skip 0;
   assert((void*)environ == (void*)p);

   printf("Environment: \n");
   while(*p)
   {
      printf("\t%s\n", (char *)(*p));
      p++;
   }
   p++; // skip 0;

   printf("Auxiliary Vectors:\n");
   Elf64_auxv_t* aux = (Elf64_auxv_t*)p;
   while(aux->a_type != AT_NULL)
   {
      // Type is defined in /usr/include/x86_64-linux-gnu/bits/auxv.h
      printf("\tType: %02ld, Value: %lx\n", aux->a_type, aux->a_un.a_val);
      aux++;
   }

   return 0;
}
