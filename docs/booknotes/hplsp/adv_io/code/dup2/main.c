#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

const char msg1[] = "hello, world #1\n";
const char msg2[] = "hello, world #2\n";

int main()
{
   int fd = open("dup2.log", O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   assert(fd > 0);

   int stdout_fd = 1;
   int rc = dup2(fd, stdout_fd);
   assert(rc != -1);

   printf("%s", msg1); // print to log file, not the stdout
   fflush(stdout); // no flush will lose the data
   write(fd, msg2, sizeof(msg2) - 1);

   close(stdout_fd);
   close(fd);

   return 0;
}
