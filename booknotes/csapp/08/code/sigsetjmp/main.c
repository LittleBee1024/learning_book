#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h> // sigjmp_buf

sigjmp_buf buf;

#define SIG_JMP_VAL 1

int setSignal(int signum, sighandler_t handler)
{
   struct sigaction action;

   action.sa_handler = handler;
   sigemptyset(&action.sa_mask); /* Block sigs of type being handled */
   action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

   return sigaction(signum, &action, NULL);
}

void handler(int sig)
{
   siglongjmp(buf, SIG_JMP_VAL);
}

int main()
{
   switch (sigsetjmp(buf, SIG_JMP_VAL))
   {
   case 0:
      setSignal(SIGINT, handler);
      printf("starting\n");
      break;
   case SIG_JMP_VAL:
      printf("restarting\n");
      break;
   default:
      printf("Unknown error condition in foo\n");
   }

   while (1)
   {
      sleep(1);
      printf("processing...\n");
   }

   return 0;
}
