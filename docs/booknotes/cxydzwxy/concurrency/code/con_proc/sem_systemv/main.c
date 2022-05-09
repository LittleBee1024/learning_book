#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <assert.h>

#include <sys/ipc.h>
#include <sys/sem.h>

// semun is not defined in "sys/sem.h"
union semun
{
   int val;              /* used for SETVAL only */
   struct semid_ds *buf; /* used for IPC_STAT and IPC_SET */
   ushort *array;        /* used for GETALL and SETALL */
   struct seminfo* __buf;
};

// op为-1时执行P操作，op为1时执行V操作
void pv(int sem_id, int op)
{
   struct sembuf sem_b;
   sem_b.sem_num = 0;
   sem_b.sem_op = op;
   sem_b.sem_flg = SEM_UNDO;
   semop(sem_id, &sem_b, 1);
}

void child_process(int semid)
{
   printf("[Child PID %d] Entered..\n", getpid());

   pv(semid, -1);
   // critical section
   sleep(1);
   printf("[Child PID %d] Just Exiting...\n", getpid());
   pv(semid, 1);
}

void parent_process(int semid)
{
   printf("[Parent PID %d] Entered..\n", getpid());

   pv(semid, -1);
   // critical section
   sleep(1);
   printf("[Parent PID %d] Just Exiting...\n", getpid());
   pv(semid, 1);
}

int main()
{
   key_t key = ftok(".", 'a');
   assert(key != -1);

   int semid = semget(key, 1, 0666 | IPC_CREAT);
   assert(key != -1);

   union semun arg;
   arg.val = 1;
   semctl(semid, 0, SETVAL, arg);

   pid_t pid = fork();
   if (pid == 0)
   {
      child_process(semid);
      return 0;
   }

   assert(pid > 0);
   parent_process(semid);

   wait(NULL);

   return 0;
}
