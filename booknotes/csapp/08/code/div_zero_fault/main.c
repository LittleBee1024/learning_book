#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void sig_handler(int signo)
{
   static int num = 0;
   if (signo == SIGFPE)
   {
      printf("Divide by zero fault happens %d times\n", num++);
   }

   const int SIGFPE_NUM = 5;
   if (num == SIGFPE_NUM)
   {
      printf("Abort the program after %d divide by zero fault\n", num);
      std::abort();
   }
}

int main()
{
   signal(SIGFPE, sig_handler);

   printf("Trigger divide by zero fault\n");
   int a = 1;
   int b = 0;
   a = a / b;

   return 0;
}
