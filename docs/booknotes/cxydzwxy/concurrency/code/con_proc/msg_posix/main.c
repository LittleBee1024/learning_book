#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <errno.h>

#define QUEUE_NAME "/my_msq_test"
#define MAX_SIZE 1024

const char buf[] = "Hello World";

void read_data()
{
   mqd_t mq = mq_open(QUEUE_NAME, O_RDONLY);
   assert(mq != -1);

   struct mq_attr attr;
   int rc = mq_getattr(mq, &attr);
   assert(rc != -1);

   char rcvbuf[attr.mq_msgsize];
   int bytes = mq_receive(mq, rcvbuf, attr.mq_msgsize, NULL);
   assert(bytes != -1);

   printf("Number of bytes of the messages in the queue is %d\n", bytes);
   printf("Read from message queue: %s\n", rcvbuf);

   rc = mq_close(mq);
   assert(rc != -1);
}

int main(int argc, char *argv[])
{
   mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, NULL);
   assert(mq != -1);
   printf("msq(%d) /dev/mqueue%s is created\n", mq, QUEUE_NAME);

   int rc = mq_send(mq, buf, sizeof(buf), 0);
   assert(rc == 0);
   rc = mq_close(mq);
   assert(rc != -1);

   pid_t pid = fork();
   if (pid == 0)
   {
      read_data();
      return 0;
   }

   assert(pid > 0);
   wait(NULL);

   // rc = mq_unlink(QUEUE_NAME);
   // assert(rc != -1);

   return 0;
}
