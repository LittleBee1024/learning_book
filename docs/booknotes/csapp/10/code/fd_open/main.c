#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

int main()
{
   const char *sharedFile = "tmp.log";

   int fd1 = open(sharedFile, O_CREAT|O_RDWR, 0777);
   assert(fd1 != -1);
   int rc = write(fd1, "hello", 5);
   assert(rc == 5);
   struct stat file1Stat;
   rc = fstat(fd1, &file1Stat);
   assert(rc != -1);
   printf("Open file %s: fd1=%d, cur_pos=%ld, inode:%ld\n",
      sharedFile, fd1, lseek(fd1, 0, SEEK_CUR), file1Stat.st_ino);

   int fd2 = open(sharedFile, O_RDONLY, 0777);
   struct stat file2Stat;
   rc = fstat(fd2, &file2Stat);
   assert(rc != -1);
   printf("Open file %s: fd2=%d, cur_pos=%ld, inode:%ld\n",
      sharedFile, fd2, lseek(fd2, 0, SEEK_CUR), file1Stat.st_ino);

   return 0;
}
