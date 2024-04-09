#include <stdio.h>
#include <unistd.h>

const char msg1[] = "hello, world #1\n";
const char msg2[] = "hello, world #2\n";

int main()
{
   int stdout_fd = 1;
   int stdout_fd_copy = dup(stdout_fd);

   printf("%s", msg1);
   write(stdout_fd_copy, msg2, sizeof(msg2) - 1); // print to stdout

   close(stdout_fd_copy);
   return 0;
}
