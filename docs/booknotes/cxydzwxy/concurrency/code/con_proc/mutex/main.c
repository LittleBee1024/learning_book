#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>

#define ASSERT(A, M, ...)                                                                  \
   if (!(A))                                                                               \
   {                                                                                       \
      fprintf(stderr, "[ERROR] (%s:%d:%s - errno: %s) " M "\n",                            \
              __FILE__, __LINE__, __func__, errno ? strerror(errno) : "0", ##__VA_ARGS__); \
      exit(EXIT_FAILURE);                                                                  \
   }

#define ASSERT_EN(en, M, ...)                                                \
   if (en != 0)                                                              \
   {                                                                         \
      errno = en;                                                            \
      fprintf(stderr, "[ERROR] (%s:%d:%s: errno: %s) " M "\n",               \
              __FILE__, __LINE__, __func__, strerror(errno), ##__VA_ARGS__); \
      exit(EXIT_FAILURE);                                                    \
   }

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
   int rc = 0;

   // init shared lock at anoymous shared memory
   shm_lock = (pthread_mutex_t*) mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
   ASSERT(shm_lock != MAP_FAILED, "Falied to mmap");

   pthread_mutexattr_t attr;
   rc = pthread_mutexattr_init(&attr);
   ASSERT_EN(rc, "Failed to pthread_mutexattr_init");

   // PTHREAD_PROCESS_PRIVATE will hange because the lock cannot be aquired if it was aquired by another process
   rc = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
   ASSERT_EN(rc, "Failed to pthread_mutexattr_setpshared");

   rc = pthread_mutex_init(shm_lock, &attr);
   ASSERT_EN(rc, "Failed to pthread_mutex_init");

   rc = pthread_mutexattr_destroy(&attr);
   ASSERT_EN(rc, "Failed to pthread_mutexattr_destroy");

   pid_t pid = fork();
   ASSERT(pid >= 0, "Failed to fork");

   if (pid == 0)
   {
      child_start();
      return 0;
   }

   parent_start();
   rc = wait(NULL);
   ASSERT(rc != -1, "Failed to wait");

   rc = pthread_mutex_destroy(shm_lock);
   ASSERT_EN(rc, "Failed to pthread_mutex_destroy");

   return 0;
}
