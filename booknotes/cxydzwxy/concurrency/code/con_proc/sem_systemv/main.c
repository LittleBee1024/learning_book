#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <assert.h>

#include <sys/ipc.h>
#include <sys/sem.h>

#define PATHNAME "." // the file must refer to an existing, accessible file
#define PROJ_ID 'a'

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

void child_process()
{
   key_t key = ftok(PATHNAME, PROJ_ID);
   int semid = semget(key, 1, 0666);
   assert(semid != -1);

   pv(semid, -1);

   printf("[Child PID %d] Critical section start...\n", getpid());
   sleep(1);
   printf("[Child PID %d] Critical section end...\n", getpid());

   pv(semid, 1);
}

void parent_process()
{
   key_t key = ftok(PATHNAME, PROJ_ID);
   int semid = semget(key, 1, 0666);
   assert(semid != -1);

   pv(semid, -1);

   printf("[Parent PID %d] Critical section start...\n", getpid());
   sleep(1);
   printf("[Parent PID %d] Critical section end...\n", getpid());

   pv(semid, 1);
}

int main()
{
   key_t key = ftok(PATHNAME, PROJ_ID);
   assert(key != -1);
   int semid = semget(key, 1, 0666 | IPC_CREAT);
   assert(semid != -1);

   printf("sem key (0x%x), sem id (%d) is created\n", key, semid);

   union semun arg;
   arg.val = 1;
   semctl(semid, 0, SETVAL, arg);

   pid_t pid = fork();
   if (pid == 0)
   {
      child_process();
      return 0;
   }

   assert(pid > 0);
   parent_process();

   wait(NULL);
   //remove sem
   //semctl(semid, 0, IPC_RMID, arg);

   return 0;
}
