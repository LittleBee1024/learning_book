#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

int main()
{
   const char *file = "tmp.log";

   int fd1 = open(file, O_CREAT | O_EXCL, 0777);
   assert(fd1 != -1);
   printf("Create file %s: %d\n", file, fd1);
   close(fd1);

   int fd2 = open(file, O_CREAT | O_EXCL);
   assert(fd2 == -1);
   printf("Failed to create file %s: %s\n", file, strerror(errno));

   int fd3 = open(file, O_CREAT, 0777);
   assert(fd3 != -1);
   printf("Open file %s: %d\n", file, fd3);

   return 0;
}
