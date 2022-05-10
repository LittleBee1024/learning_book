#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#define FILE_PATH "/sem_test"

void child_process()
{
   sem_t *sem = sem_open(FILE_PATH, O_RDWR);
   sem_wait(sem);

   printf("[Child PID %d] Critical section start...\n", getpid());
   sleep(1);
   printf("[Child PID %d] Critical section end...\n", getpid());

   sem_post(sem);
   sem_close(sem);
}

void parent_process()
{
   sem_t *sem = sem_open(FILE_PATH, O_RDWR);
   sem_wait(sem);

   printf("[Parent PID %d] Critical section start...\n", getpid());
   sleep(1);
   printf("[Parent PID %d] Critical section end...\n", getpid());

   sem_post(sem);
   sem_close(sem);
}

int main()
{
   // create /dev/shm/sem.sem_test
   sem_t *sem = sem_open(FILE_PATH, O_RDWR | O_CREAT, 0644, 1);
   if (sem == SEM_FAILED)
   {
      perror("sem_open");
      return -1;
   }
   int val = 0;
   int rc = sem_getvalue(sem, &val);
   assert(rc == 0);
   printf("sem value = %d\n", val);
   sem_close(sem);

   pid_t pid = fork();
   if (pid == 0)
   {
      child_process();
      return 0;
   }

   assert(pid > 0);
   parent_process();

   wait(NULL);

   // remove /dev/shm/sem.sem_test
   // sem_unlink(FILE_PATH);

   return 0;
}
