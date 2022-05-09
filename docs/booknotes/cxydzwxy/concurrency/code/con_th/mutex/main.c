#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

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

struct thread_info
{
   pthread_t thread_id;
   int thread_num;
};

pthread_mutex_t lock;

void *thread_start(void *arg)
{
   pthread_mutex_lock(&lock);

   struct thread_info *tinfo = arg;
   printf("Job %d has started\n", tinfo->thread_num);
   sleep(1);
   printf("Job %d has finished\n", tinfo->thread_num);

   pthread_mutex_unlock(&lock);

   return NULL;
}

int main(void)
{
   int rc = 0;
   rc = pthread_mutex_init(&lock, NULL);
   ASSERT_EN(rc, "Failed to pthread_mutex_init");

   // Allocate memory for pthread_create() arguments.
   const int NUM_THREADS = 2;
   struct thread_info *tinfo = malloc(NUM_THREADS * sizeof(*tinfo));
   ASSERT(tinfo != NULL, "Failed to malloc");

   for (int tnum = 0; tnum < NUM_THREADS; tnum++)
   {
      tinfo[tnum].thread_num = tnum + 1;
      rc = pthread_create(&tinfo[tnum].thread_id, NULL, &thread_start, &tinfo[tnum]);
      ASSERT_EN(rc, "Failed to pthread_create");
   }

   for (int tnum = 0; tnum < NUM_THREADS; tnum++)
   {
      rc = pthread_join(tinfo[tnum].thread_id, NULL);
      ASSERT_EN(rc, "Failed to pthread_join");
   }

   rc = pthread_mutex_destroy(&lock);
   ASSERT_EN(rc, "Failed to pthread_mutex_destroy");

   return 0;
}
