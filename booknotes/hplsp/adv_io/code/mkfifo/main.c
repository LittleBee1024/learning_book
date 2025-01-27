#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>

#define NAMED_FIFO "/tmp/fifo"
#define MSGSIZE 16
const char *msg1 = "hello, world #1";
const char *msg2 = "hello, world #2";

void parent_rw()
{
   printf("[Parent] Task Starts\n");

   int fd_read = open(NAMED_FIFO, O_RDONLY);
   assert(fd_read > 0);

   char inbuf[MSGSIZE];
   int nbytes = read(fd_read, inbuf, MSGSIZE);
   printf("[Parent] read %d bytes to from child process: %s\n", nbytes, inbuf);

   close(fd_read);

   int fd_write = open(NAMED_FIFO, O_WRONLY);
   assert(fd_write > 0);

   nbytes = write(fd_write, msg1, MSGSIZE);
   printf("[Parent] wrote %d bytes to child process: %s\n", nbytes, msg1);

   close(fd_write);

   printf("[Parent] Task Ends\n");
}

void child_wr()
{
   printf("[Child] Task Starts\n");

   int fd_write = open(NAMED_FIFO, O_WRONLY);
   assert(fd_write > 0);

   int nbytes = write(fd_write, msg2, MSGSIZE);
   printf("[Child] wrote %d bytes to parent process: %s\n", nbytes, msg2);

   close(fd_write);

   int fd_read = open(NAMED_FIFO, O_RDONLY);
   assert(fd_read > 0);

   char inbuf[MSGSIZE];
   nbytes = read(fd_read, inbuf, MSGSIZE);
   printf("[Child] read %d bytes to from parent process: %s\n", nbytes, inbuf);

   close(fd_read);

   printf("[Child] Task Ends\n");
}

int main(int argc, char **argv)
{
   int rc = mkfifo(NAMED_FIFO, 0777);
   if (rc == -1)
   {
      perror(NAMED_FIFO);
      return -1;
   }

   pid_t pid = fork();
   assert(pid >= 0);

   if (pid == 0)
   {
      child_wr();
      return 0;
   }

   sleep(1);
   parent_rw();

   rc = wait(NULL);
   assert(rc != -1);

   // remove named fifo, otherwise mkfifo reports "File Exists" error.
   rc = unlink(NAMED_FIFO);
   assert(rc != -1);
   return 0;
}
