#include "./static.h"
#include <stdio.h>

static int s_a;

void print_static_var_addr()
{
   printf("s_a addr = %p\n", &s_a);
}
