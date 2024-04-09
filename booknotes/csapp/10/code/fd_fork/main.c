#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>

int main()
{
   const char *sharedFile = "tmp.log";

   int fd1 = open(sharedFile, O_CREAT|O_RDWR, 0777);
   assert(fd1 != -1);
   int rc = write(fd1, "hello", 5);
   assert(rc == 5);
   printf("Open file %s: fd1=%d, cur_pos=%ld\n", sharedFile, fd1, lseek(fd1, 0, SEEK_CUR));

   if (fork() == 0)
   {
      printf("[Child %d] fd1=%d, cur_pos=%ld\n", getpid(), fd1, lseek(fd1, 0, SEEK_CUR));
      int rc = write(fd1, " world", 6);
      assert(rc == 6);
      return 0;
   }
   wait(NULL);
   printf("[Parent %d] fd1=%d, cur_pos=%ld\n", getpid(), fd1, lseek(fd1, 0, SEEK_CUR));

   return 0;
}
