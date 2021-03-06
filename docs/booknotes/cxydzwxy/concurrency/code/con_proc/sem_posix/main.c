#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/mman.h>

sem_t *sem;

void child_process()
{
   sem_wait(sem);

   printf("Child process has started\n");
   sleep(1);
   printf("Child process has finished\n");

   sem_post(sem);
}

void parent_process()
{
   sem_wait(sem);

   printf("Parent process has started\n");
   sleep(1);
   printf("Parent process has finished\n");

   sem_post(sem);
}

int main()
{
   sem = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
   assert(sem != MAP_FAILED);

   /* 1: shared across processes
    * 0: initial value, wait locked until one post happens (making it > 0)
    */
   sem_init(sem, 1, 1);

   pid_t pid = fork();
   if (pid == 0)
   {
      child_process();
      return 0;
   }

   assert(pid > 0);
   parent_process();

   wait(NULL);
   sem_destroy(sem);

   return 0;
}
