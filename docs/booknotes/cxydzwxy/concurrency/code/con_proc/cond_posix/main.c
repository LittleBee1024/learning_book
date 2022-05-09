#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/mman.h>

pthread_mutex_t *shm_lock;
pthread_cond_t *shm_cond;

void run_process()
{
   pthread_mutex_lock(shm_lock);

   printf("[Run PID %d] Entered..\n", getpid());

   pthread_cond_wait(shm_cond, shm_lock);

   printf("[Run PID %d] Just Exiting...\n", getpid());

   pthread_mutex_unlock(shm_lock);
}

void trigger_process()
{
   pthread_mutex_lock(shm_lock);

   printf("[Trigger PID %d] Entered..\n", getpid());

   pthread_cond_broadcast(shm_cond);

   printf("[Trigger PID %d] Just Exiting...\n", getpid());

   pthread_mutex_unlock(shm_lock);
}

int main()
{
   shm_lock = (pthread_mutex_t*) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
   assert(shm_lock != MAP_FAILED);

   shm_cond = (pthread_cond_t*) mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
   assert(shm_cond != MAP_FAILED);

   pthread_mutexattr_t mutex_attr;
   pthread_mutexattr_init(&mutex_attr);
   pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
   pthread_mutex_init(shm_lock, &mutex_attr);
   pthread_mutexattr_destroy(&mutex_attr);

   pthread_condattr_t cond_attr;
   pthread_condattr_init(&cond_attr);
   pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
   pthread_cond_init(shm_cond, &cond_attr);
   pthread_condattr_destroy(&cond_attr);

   pid_t run0_pid = fork();
   if (run0_pid == 0)
   {
      run_process();
      return 0;
   }

   assert(run0_pid > 0);
   pid_t run1_pid = fork();
   if (run1_pid == 0)
   {
      run_process();
      return 0;
   }

   sleep(2);
   trigger_process();

   wait(NULL);
   wait(NULL);

   return 0;
}
