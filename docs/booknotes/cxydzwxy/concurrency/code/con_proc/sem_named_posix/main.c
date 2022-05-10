#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

sem_t *sem;

void child_process()
{
   sem_wait(sem);
   printf("[Child PID %d] Entered..\n", getpid());

   // critical section
   sleep(1);

   printf("[Child PID %d] Just Exiting...\n", getpid());
   sem_post(sem);
}

void parent_process()
{
   sem_wait(sem);
   printf("[Parent PID %d] Entered..\n", getpid());

   // critical section
   sleep(1);

   printf("[Parent PID %d] Just Exiting...\n", getpid());
   sem_post(sem);
}

int main()
{
   sem = sem_open("hello", O_RDWR | O_CREAT, 0644, 1);
   if (sem == SEM_FAILED)
   {
      perror("sem_open");
      return -1;
   }
   int val = 0;
   int rc = sem_getvalue(sem, &val);
   assert(rc == 0);
   printf("sem value = %d\n", val);

   pid_t pid = fork();
   if (pid == 0)
   {
      child_process();
      return 0;
   }

   assert(pid > 0);
   parent_process();

   wait(NULL);
   sem_close(sem);

   // remove /dev/shm/sem.hello
   // sem_unlink("hello");

   return 0;
}
