#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <assert.h>

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
   int childPID = 0;
   while((childPID = waitpid(-1, NULL, 0)) > 0)
   {
      printf("[Signal] Handle %d signal from child %d\n", sig, childPID);
   }
   // extend the time of signal handler
   sleep(1);
}

int main()
{
   int rc = setSignal(SIGCHLD, handler);
   assert(rc == 0);

   for (int i = 0; i < 3; i++)
   {
      if (fork() == 0)
      {
         printf("[Child] Hello from child %d\n", (int)getpid());
         return 0;
      }
   }

   printf("[Parent] Processing\n");
   int i = 2;
   while (i--)
   {
      sleep(1);
   }

   return 0;
}
