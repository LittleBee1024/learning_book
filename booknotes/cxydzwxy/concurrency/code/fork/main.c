#define _GNU_SOURCE
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int child_func(char* buf)
{
   printf("[PID %d, TID %d] Child sees buf = \"%s\"\n", getpid(), gettid(), buf);
   strcpy(buf, "hello from child");
   return 0;
}

int main(int argc, char **argv)
{
   char buf[100];
   strcpy(buf, "hello from parent");

   pid_t pid = fork();

   if (pid < 0)
   {
      perror("fork");
      return -1;
   }

   if (pid == 0)
   {
      // chile process
      child_func(buf);
      return 0;
   }

   // parent process
   int status;
   if (wait(&status) == -1)
   {
      perror("wait");
      return -1;
   }

   printf("[PID %d, TID %d] Child exited with status %d, buf = \"%s\"\n", getpid(), gettid(), status, buf);
   return 0;
}