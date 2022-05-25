#include <stdio.h>
#include <unistd.h>

const char msg1[] = "hello, world #1\n";

int main()
{
   int stdout_fd = 1;
   int stdout_fd_copy = dup(stdout_fd);
   write(stdout_fd_copy, msg1, sizeof(msg1) - 1); // print to stdout

   close(stdout_fd_copy);
   return 0;
}
