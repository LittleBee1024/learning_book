#include <cassert>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void SIGUSR1_handler(int signo)
{
   assert(signo == SIGUSR1);
   printf(">>> Handle SIGUSR1 <<<\n");
}

int main()
{
   printf("Catch signal example\n");
   signal(SIGUSR1, SIGUSR1_handler);

   int id = fork();
   if (id > 0)
   {
      /*parent process*/
      printf("This is parent process to handle SIGUSR1 signal [process PID: %d, signal sender PID: %d].\n",
         getpid(), id);

      int status;
      waitpid(id, &status, 0);
      (status == 0) ? printf("The child process terminated normally.\n") : printf("The child process terminated with an error!\n");

      printf("Parent process is Done!\n");
      return 0;
   }
   else if (id == 0)
   {
      /*child process*/
      printf("This is child process to send SIGUSR1 signal [process PID: %d, parent process PID: %d].\n",
         getpid(), getppid());

      int ret = kill(getppid(), SIGUSR1);
      (ret == 0) ? printf("Send SIGUSR1 signal successfully.\n") : printf("Fail to send SIGUSR1 signal.\n");

      printf("Child process is Done!\n");
      return 0;
   }

   printf("fork creation failed!!!\n");
   return -1;
}
