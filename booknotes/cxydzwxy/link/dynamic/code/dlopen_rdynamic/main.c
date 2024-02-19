// https://stackoverflow.com/questions/36692315/what-exactly-does-rdynamic-do-and-when-exactly-is-it-needed

#include "static.h"
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char* argv[])
{
   printf("Call static function directly:\n");
   printf("print_static_var_addr addr: %p\n", print_static_var_addr);
   print_static_var_addr();

   void *handle = dlopen("./libshared.so", RTLD_NOW);
   if (handle == NULL)
   {
      printf("Open library %s error: %s\n", argv[1], dlerror());
      return -1;
   }

   void (*func)(void);
   func = dlsym(handle, "call_static_func_from_shared_lib");

   char* error;
   if ((error = dlerror()) != NULL)
   {
      printf("Symbol call_static_func_from_shared_lib not found: %s\n", error);
      dlclose(handle);
      return -1;
   }

   printf("Call static function from dlopen:\n");
   func();
   dlclose(handle);

   return 0;
}
