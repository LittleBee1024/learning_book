#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
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
{                       /* Used as argument to thread_start() */
   pthread_t thread_id; /* ID returned by pthread_create() */
   int thread_num;      /* Application-defined thread # */
   int ret;             /* Return value */
};

void *thread_start(void *arg)
{
   struct thread_info *tinfo = arg;

   printf("[Thread_%d, num_%ld] Thread processing done\n", tinfo->thread_num, tinfo->thread_id);
   tinfo->ret = tinfo->thread_num;
   pthread_exit(&tinfo->ret);
}

int main(void)
{
   // Allocate memory for pthread_create() arguments.
   const int NUM_THREADS = 2;
   struct thread_info *tinfo = malloc(NUM_THREADS * sizeof(*tinfo));
   ASSERT(tinfo != NULL, "Failed to malloc");

   int rc = 0;
   for (int tnum = 0; tnum < NUM_THREADS; tnum++)
   {
      tinfo[tnum].thread_num = tnum + 1;
      rc = pthread_create(&tinfo[tnum].thread_id, NULL, &thread_start, &tinfo[tnum]);
      ASSERT_EN(rc, "Failed to pthread_create");
   }

   void *res;
   for (int tnum = 0; tnum < NUM_THREADS; tnum++)
   {
      rc = pthread_join(tinfo[tnum].thread_id, &res);
      ASSERT_EN(rc, "Failed to pthread_join");

      printf("Joined with thread %d; Return value from thread is [%d]\n",
             tinfo[tnum].thread_num, *(int *)res);
      // free res if the res is allocated by the thread
   }

   free(tinfo);

   return 0;
}
