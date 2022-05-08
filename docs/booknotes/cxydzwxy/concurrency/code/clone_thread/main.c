// https://stackoverflow.com/questions/15858207/how-to-create-a-real-thread-with-clone-on-linux

// We have to define the _GNU_SOURCE to get access to clone(2) and the CLONE_*
// flags from sched.h
#define _GNU_SOURCE
#include <sched.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STACK_SIZE 4096

int func(void *arg)
{
   printf("Inside func.\n");
   sleep(1);
   printf("[PID %d, TID %d] Terminating func...\n", getpid(), gettid());

   return 0;
}

int main()
{
   printf("This process pid: %d, tid: %d\n", getpid(), gettid());
   // check "Threads" part in the status file, it should be 2
   char status_file[] = "/proc/self/status";
   void *child_stack = malloc(STACK_SIZE);
   int thread_pid;

   printf("Creating new thread...\n");
   thread_pid = clone(&func, (char *)child_stack + STACK_SIZE, CLONE_THREAD | CLONE_SIGHAND | CLONE_FS | CLONE_VM | CLONE_FILES, NULL);
   printf("Done! Thread pid: %d\n", thread_pid);

   FILE *fp = fopen(status_file, "rb");

   printf("Looking into %s...\n", status_file);

   while (1)
   {
      char ch = fgetc(fp);
      if (feof(fp))
         break;
      printf("%c", ch);
   }

   fclose(fp);

   // block for thread end
   getchar();
   printf("[PID %d, TID %d] Parent process end\n", getpid(), gettid());

   return 0;
}
