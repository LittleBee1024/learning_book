#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

int main()
{
   const char *sharedFile = "tmp.log";

   int fd1 = open(sharedFile, O_CREAT|O_RDWR, 0777);
   assert(fd1 != -1);
   int rc = write(fd1, "hello", 5);
   assert(rc == 5);
   printf("Open file %s: fd=%d, cur_pos=%ld\n", sharedFile, fd1, lseek(fd1, 0, SEEK_CUR));

   int fd2 = dup(fd1);
   assert(fd2 != -1);
   printf("Dup: fd=%d, cur_pos=%ld\n", fd2, lseek(fd2, 0, SEEK_CUR));

   int fd3 = 6;
   fd3 = dup2(fd1, fd3);
   assert(fd3 == 6);
   printf("Dup2: fd=%d, cur_pos=%ld\n", fd3, lseek(fd3, 0, SEEK_CUR));

   int fd4 = fcntl(fd1, F_DUPFD, fd3);
   assert(fd4 == fd3+1);
   printf("fcntl(F_DUPFD): fd4=%d, cur_pos=%ld\n", fd4, lseek(fd4, 0, SEEK_CUR));

   return 0;
}
