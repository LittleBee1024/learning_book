#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <assert.h>

#define MSGSIZE 16
const char *msg1 = "hello, world #1";
const char *msg2 = "hello, world #2";

void parent_rw(int socket)
{
   printf("[Parent] Task Starts\n");

   char inbuf[MSGSIZE];
   int nbytes = read(socket, inbuf, MSGSIZE);
   printf("[Parent] read %d bytes to from child process: %s\n", nbytes, inbuf);

   nbytes = write(socket, msg1, MSGSIZE);
   printf("[Parent] wrote %d bytes to child process: %s\n", nbytes, msg1);

   printf("[Parent] Task Ends\n");
}

void child_wr(int socket)
{
   printf("[Child] Task Starts\n");

   int nbytes = write(socket, msg2, MSGSIZE);
   printf("[Child] wrote %d bytes to parent process: %s\n", nbytes, msg2);

   char inbuf[MSGSIZE];
   nbytes = read(socket, inbuf, MSGSIZE);
   printf("[Child] read %d bytes to from parent process: %s\n", nbytes, inbuf);

   printf("[Child] Task Ends\n");
}

int main()
{
   int fd[2];
   int rc = socketpair(PF_LOCAL, SOCK_STREAM, 0, fd);
   assert(rc != -1);

   const int parentSocket = 0;
   const int childSocket = 1;

   pid_t pid = fork();
   assert(pid >= 0);

   if (pid == 0)
   {
      close(fd[parentSocket]);

      child_wr(fd[childSocket]);
      close(fd[childSocket]);
      return 0;
   }

   close(fd[childSocket]);
   sleep(1);

   parent_rw(fd[parentSocket]);
   close(fd[parentSocket]);

   rc = wait(NULL);
   assert(rc != -1);

   return 0;
}
