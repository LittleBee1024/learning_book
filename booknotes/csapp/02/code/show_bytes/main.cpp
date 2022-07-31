#include <stdio.h>
#include <typeinfo>

typedef unsigned char *byte_pointer;

void show_bytes(byte_pointer start, size_t len)
{
   for (size_t i = 0; i < len; i++)
      printf(" %.2x", start[i]);
   printf("\n");
}

template <typename T>
void show(T x)
{
   show_bytes((byte_pointer)&x, sizeof(x));
}

int main()
{
   int ival = 12345;
   float fval = (float)ival;
   int *pval = &ival;
   show(ival);
   show(fval);
   show(pval);

   const char s[] = "abcd";
   show_bytes((byte_pointer)s, sizeof(s));
   return 0;
}
