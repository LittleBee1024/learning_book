#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;

void *thread_start(void *arg)
{
   // wait
   sem_wait(&sem);

   pthread_t id = pthread_self();
   printf("[Thread %ld] Entered..\n", id);

   // critical section
   sleep(1);

   // signal
   printf("[Thread %ld] Just Exiting...\n", id);
   sem_post(&sem);

   return NULL;
}

int main(void)
{
   sem_init(&sem, 0, 1);
   pthread_t t1, t2;
   pthread_create(&t1, NULL, thread_start, NULL);
   pthread_create(&t2, NULL, thread_start, NULL);
   pthread_join(t1, NULL);
   pthread_join(t2, NULL);
   sem_destroy(&sem);

   return 0;
}
