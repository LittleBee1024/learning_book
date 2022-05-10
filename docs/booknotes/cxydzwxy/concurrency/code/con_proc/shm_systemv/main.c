#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#define PATHNAME "." // the file must refer to an existing, accessible file
#define PROJ_ID 'a'

#define SHMSZ 27
const char buf[] = "Hello World";

void read_data()
{
   key_t key = ftok(PATHNAME, PROJ_ID);
   int shmid = shmget(key, 0, 0444);
   assert(shmid != -1);

   struct shmid_ds buf;
   int res = shmctl(shmid, IPC_STAT, &buf);
   assert(res == 0);
   const size_t shm_size = buf.shm_segsz;
   printf("The size of shard memory is %zu bytes\n", shm_size);

   void *addr = shmat(shmid, NULL, 0);
   char data[shm_size];
   memcpy(data, addr, shm_size);
   printf("Read from shared memory: \"%s\"\n", data);
   shmdt(addr);
}

int main(void)
{
   key_t key = ftok(PATHNAME, PROJ_ID);
   assert(key != -1);
   int shmid = shmget(key, SHMSZ, 0666 | IPC_CREAT);
   assert(shmid != -1);

   printf("shm key (0x%x), shm id (%d) is created\n", key, shmid);

   // attach the segment to our data space.
   void *addr = shmat(shmid, NULL, 0);
   memcpy(addr, buf, sizeof(buf));

   pid_t pid = fork();
   if (pid == 0)
   {
      read_data();
      return 0;
   }

   assert(pid > 0);
   wait(NULL);

   // detache
   int rc = shmdt(addr);
   assert(rc != -1);

   // delete
   // shmctl(shmid, IPC_RMID, NULL);

   return 0;
}
