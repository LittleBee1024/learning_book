#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MSGSIZE 16
const char *msg1 = "hello, world #1";
const char *msg2 = "hello, world #2";

void write_data(int write_fd)
{
   printf("Parent Task Starts\n");

   // write will trigger SIGPIPE signal if all read fd is closed
   int nbytes = write(write_fd, msg1, MSGSIZE);
   printf("parent process wrote %d bytes to child process: %s\n", nbytes, msg1);
   nbytes = write(write_fd, msg2, MSGSIZE);
   printf("parent process wrote %d bytes to child process: %s\n", nbytes, msg2);

   printf("Parent Task Ends\n");
}

void read_data(int read_fd)
{
   printf("Child Task Starts\n");

   char inbuf[MSGSIZE];
   int nbytes = 0;

   // read will return zero if all write fd is closed
   while ((nbytes = read(read_fd, inbuf, MSGSIZE)) > 0)
      printf("child process read %d bytes to from parent process: %s\n", nbytes, inbuf);

   printf("Child Task Ends\n");
}

int main(int argc, char **argv)
{
   int fd[2];
   int rc = pipe(fd);
   assert(rc != -1);

   int read_fd = fd[0];
   int write_fd = fd[1];

   pid_t pid = fork();
   assert(pid >= 0);

   if (pid == 0)
   {
      close(write_fd);

      read_data(read_fd);
      close(read_fd);
      return 0;
   }

   close(read_fd);
   sleep(1);

   write_data(write_fd);
   close(write_fd);

   rc = wait(NULL);
   assert(rc != -1);
   return 0;
}
