#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <assert.h>

void initjobs()
{
   printf("[Global] Init jobs\n");
}

void addjob(int pid)
{
   printf("[Global] Add %d PID to jobs\n", pid);
}

void deletejob(int pid)
{
   printf("[Global] Remove %d PID from jobs\n", pid);
}

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
   sigset_t mask_all, mask_recover;
   sigfillset(&mask_all);
   int pid = 0;
   while ((pid = waitpid(-1, NULL, 0)) > 0)
   {
       /* Reap a zombie child */
      sigprocmask(SIG_BLOCK, &mask_all, &mask_recover);
      deletejob(pid); /* Delete the child from the job list */
      sigprocmask(SIG_SETMASK, &mask_recover, NULL);
   }
   // extend the time of signal handler
   sleep(1);
}

int main(int argc, char **argv)
{
   sigset_t mask_all, mask_recover;
   sigfillset(&mask_all);

   sigset_t mask_chld;
   sigemptyset(&mask_chld);
   sigaddset(&mask_chld, SIGCHLD);

   int rc = setSignal(SIGCHLD, handler);
   assert(rc == 0);

   initjobs();

   for (int i = 0; i < 3; i++)
   {
      sigprocmask(SIG_BLOCK, &mask_chld, &mask_recover); /* Block SIGCHLD */
      int pid = fork();
      if (pid == 0)
      {
         /* Child process */
         sigprocmask(SIG_SETMASK, &mask_recover, NULL); /* Unblock SIGCHLD */
         printf("[Child] Hello from child %d\n", (int)getpid());
         return 0;
      }
      sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
      addjob(pid);
      sigprocmask(SIG_SETMASK, &mask_recover, NULL);  /* Unblock SIGCHLD */
   }

   printf("[Parent] Processing\n");
   int i = 2;
   while (i--)
   {
      sleep(1);
   }

   return 0;
}
