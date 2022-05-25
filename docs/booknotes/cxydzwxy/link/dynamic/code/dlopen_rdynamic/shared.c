#include "./static.h"
#include <stdio.h>

void call_static_func_from_shared_lib()
{
   printf("print_static_var_addr addr: %p\n", print_static_var_addr);
   print_static_var_addr();
}
