#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

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
   if (tinfo == NULL)
      perror("malloc");

   for (int tnum = 0; tnum < NUM_THREADS; tnum++)
   {
      tinfo[tnum].thread_num = tnum + 1;
      int rc = pthread_create(&tinfo[tnum].thread_id, NULL, &thread_start, &tinfo[tnum]);
      if (rc != 0)
         perror("pthread_create");
   }

   void *res;
   for (int tnum = 0; tnum < NUM_THREADS; tnum++)
   {
      int rc = pthread_join(tinfo[tnum].thread_id, &res);
      if (rc != 0)
         perror("pthread_join");

      printf("Joined with thread %d; Return value from thread is [%d]\n",
             tinfo[tnum].thread_num, *(int *)res);
      // free res if the res is allocated by the thread
   }

   free(tinfo);

   return 0;
}
