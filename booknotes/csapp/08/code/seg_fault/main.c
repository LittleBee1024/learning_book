#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void sig_handler(int signo)
{
   static int num = 0;
   if (signo == SIGSEGV)
   {
      printf("Segmentation fault happens %d times\n", num++);
   }

   const int SIGSEGV_NUM = 5;
   if (num == SIGSEGV_NUM)
   {
      printf("Abort the program after %d segmentation fault\n", num);
      std::abort();
   }
}


int main()
{
   signal(SIGSEGV, sig_handler);

   printf("Trigger segmentation fault\n");
   int *a = NULL;
   int b = *a;

   return 0;
}
