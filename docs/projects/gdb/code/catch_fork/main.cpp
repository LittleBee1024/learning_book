#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
   printf("Fork example\n");

   int id = fork();
   if (id > 0)
   {
      /*parent process*/
      printf("This is parent section [process id: %d, fork id: %d].\n", getpid(), id);

      int status;
      waitpid(id, &status, 0);
      (status == 0) ? printf("The child process terminated normally.\n") :
         printf("The child process terminated with an error!\n");

      printf("Parent process is Done!\n");
      return 0;
   }
   else if (id == 0)
   {
      /*child process*/
      printf("This is child section [process id: %d, parent process id: %d].\n", getpid(), getppid());
      printf("Child process is Done!\n");
      return 0;
   }

   printf("fork creation failed!!!\n");
   return -1;
}
