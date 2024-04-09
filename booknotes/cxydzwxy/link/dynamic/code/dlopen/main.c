#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char* argv[])
{
   double (*func)(double);
   char* error;

   void *handle = dlopen(argv[1], RTLD_NOW);
   if (handle == NULL)
   {
      printf("Open library %s error: %s\n", argv[1], dlerror());
      return -1;
   }

   func = dlsym(handle, "sin");
   if ((error = dlerror()) != NULL)
   {
      printf("Symbol sin not found: %s\n", error);
      dlclose(handle);
      return -1;
   }

   printf("%f\n", func(3.1415926 / 2));
   dlclose(handle);

   return 0;
}
