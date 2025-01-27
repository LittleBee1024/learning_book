#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <assert.h>

#define PATHNAME "."
#define PROJ_ID 'a'
#define MAXSIZE 128

#define USER_MESSAGE_TYPE 1 // should be greater than zero
typedef struct my_msgbuf
{
   long mtype;
   char mtext[MAXSIZE];
} my_msgbuf;

void read_msg()
{
   key_t key = ftok(PATHNAME, PROJ_ID);
   assert(key != -1);
   int msqid = msgget(key, 0444);
   assert(msqid != -1);

   struct msqid_ds buf;
   int rc = msgctl(msqid, IPC_STAT, &buf);
   assert(rc == 0);

   printf("Number of messages in the queue is %lu\n", buf.msg_qnum);
   const size_t data_bytes = buf.__msg_cbytes;
   printf("Number of bytes of the messages in the queue is %zu\n", data_bytes);

   struct my_msgbuf rcvbuffer;
   rc = msgrcv(msqid, &rcvbuffer, data_bytes, USER_MESSAGE_TYPE, 0);
   assert(rc != -1);

   printf("Read from message queue (message type %ld): %s\n", rcvbuffer.mtype, rcvbuffer.mtext);
   return;
}

int main()
{
   key_t key = ftok(PATHNAME, PROJ_ID);
   assert(key != -1);
   int msqid = msgget(key, IPC_CREAT | 0666);
   assert(msqid != -1);
   printf("msq key(0x%x), msq id(%d) is created\n", key, msqid);

   my_msgbuf sbuf;
   sbuf.mtype = USER_MESSAGE_TYPE;
   const char data[] = "Hello World";
   memcpy(sbuf.mtext, data, sizeof(data));
   size_t buflen = sizeof(data);

   int rc = msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT);
   assert(rc != -1);

   pid_t pid = fork();
   if (pid == 0)
   {
      read_msg();
      return 0;
   }

   assert(pid > 0);
   wait(NULL);

   // Remove message queue
   // rc = msgctl(msqid, IPC_RMID, NULL);
   // assert(rc != -1);

   return 0;
}
