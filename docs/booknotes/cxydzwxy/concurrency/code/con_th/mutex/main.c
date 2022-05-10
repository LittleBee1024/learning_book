#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock;

void *thread_start(void *arg)
{
   pthread_mutex_lock(&lock);

   pthread_t id = pthread_self();
   printf("[Thread %ld] Critical section start...\n", id);
   sleep(1);
   printf("[Thread %ld] Critical section end...\n", id);

   pthread_mutex_unlock(&lock);

   return NULL;
}

int main(void)
{
   pthread_mutex_init(&lock, NULL);

   pthread_t t1, t2;
   pthread_create(&t1, NULL, thread_start, NULL);
   pthread_create(&t2, NULL, thread_start, NULL);

   pthread_join(t1, NULL);
   pthread_join(t2, NULL);

   pthread_mutex_destroy(&lock);

   return 0;
}
