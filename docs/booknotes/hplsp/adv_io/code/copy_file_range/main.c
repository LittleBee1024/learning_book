#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[])
{
   if (argc != 3)
   {
      fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   int fd_in = open(argv[1], O_RDONLY);
   assert(fd_in > 0);

   struct stat stat;
   int rc = fstat(fd_in, &stat);
   assert(rc != -1);

   int len = stat.st_size;

   int fd_out = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
   assert(fd_out > 0);

   do
   {
      rc = copy_file_range(fd_in, NULL, fd_out, NULL, len, 0);
      len -= rc;
   } while (len > 0 && rc > 0);

   close(fd_in);
   close(fd_out);
   return 0;
}
