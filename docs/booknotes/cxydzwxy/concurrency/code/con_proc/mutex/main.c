#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>

// The mutex must be placed at shared memory(anoymous or file mapped) but not a global var
pthread_mutex_t *shm_lock;

void child_start()
{
   pthread_mutex_lock(shm_lock);

   printf("Child process has started\n");
   sleep(1);
   printf("Child process has finished\n");

   pthread_mutex_unlock(shm_lock);
}

void parent_start()
{
   pthread_mutex_lock(shm_lock);

   printf("Parent process has started\n");
   sleep(1);
   printf("Parent process has finished\n");

   pthread_mutex_unlock(shm_lock);
}

int main(void)
{
   // init shared lock at anoymous shared memory
   shm_lock = (pthread_mutex_t*) mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

   pthread_mutexattr_t attr;
   pthread_mutexattr_init(&attr);

   // PTHREAD_PROCESS_PRIVATE will hange because the lock cannot be aquired if it was aquired by another process
   pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
   pthread_mutex_init(shm_lock, &attr);
   pthread_mutexattr_destroy(&attr);

   pid_t pid = fork();
   if (pid == 0)
   {
      child_start();
      return 0;
   }

   parent_start();
   wait(NULL);

   pthread_mutex_destroy(shm_lock);

   return 0;
}
