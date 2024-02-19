#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock;
pthread_cond_t cond;
int started = 0;

void *thread_run(void *arg)
{
   pthread_mutex_lock(&lock);

   pthread_t id = pthread_self();
   printf("[Thread %ld] Entered..\n", id);

   if (!started)
      pthread_cond_wait(&cond, &lock);

   printf("[Thread %ld] Just Exiting...\n", id);

   pthread_mutex_unlock(&lock);

   return NULL;
}

void *thread_trigger(void *arg)
{
   pthread_mutex_lock(&lock);

   pthread_t id = pthread_self();
   printf("[Thread %ld] Start Trigger...\n", id);
   started = 1;
   pthread_cond_broadcast(&cond);
   printf("[Thread %ld] End Trigger...\n", id);

   pthread_mutex_unlock(&lock);
   return NULL;
}

int main(void)
{
   pthread_mutex_init(&lock, NULL);
   pthread_cond_init(&cond, NULL);

   pthread_t run_t0, run_t1;
   pthread_t trigger_t;
   pthread_create(&run_t0, NULL, thread_run, NULL);
   pthread_create(&run_t1, NULL, thread_run, NULL);
   sleep(1);
   pthread_create(&trigger_t, NULL, thread_trigger, NULL);

   pthread_join(trigger_t, NULL);
   pthread_join(run_t0, NULL);
   pthread_join(run_t1, NULL);

   pthread_cond_destroy(&cond);
   pthread_mutex_destroy(&lock);

   return 0;
}
