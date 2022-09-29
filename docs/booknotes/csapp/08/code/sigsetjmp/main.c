#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h> // sigjmp_buf

sigjmp_buf buf;

#define SIG_JMP_VAL 1

void handler(int sig)
{
   siglongjmp(buf, SIG_JMP_VAL);
}

typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler)
{
   struct sigaction action, old_action;

   action.sa_handler = handler;
   sigemptyset(&action.sa_mask); /* Block sigs of type being handled */
   action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

   if (sigaction(signum, &action, &old_action) < 0)
      fprintf(stderr, "Signal error");
   return (old_action.sa_handler);
}

int main()
{
   if (!sigsetjmp(buf, SIG_JMP_VAL))
   {
      Signal(SIGINT, handler);
      printf("starting\n");
   }
   else
   {
      printf("restarting\n");
   }

   while (1)
   {
      sleep(1);
      printf("processing...\n");
   }

   return 0;
}
