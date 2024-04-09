#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h> // basename
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h> // fstat
#include <string.h>   //memcpy
#include <assert.h>

int main(int argc, char *argv[])
{
   if (argc < 3)
   {
      printf("usage: %s in_filename out_filename\n", basename(argv[0]));
      return -1;
   }
   const char *in_filename = argv[1];
   const char *out_filename = argv[2];

   int fdin = open(in_filename, O_RDONLY);
   if (fdin < 0)
   {
      printf("Error opening the file: %s\n", in_filename);
      return -1;
   }

   int fdout = open(out_filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   assert(fdout > 0);

   struct stat statbuf;
   int rc = fstat(fdin, &statbuf);
   assert(rc == 0);

   void *src_ptr = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0);
   assert(src_ptr != MAP_FAILED);

   // expand the space of the output file
   rc = ftruncate(fdout, statbuf.st_size);
   assert(rc != -1);

   void *dest_ptr = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0);
   assert(dest_ptr != MAP_FAILED);

   memcpy(dest_ptr, src_ptr, statbuf.st_size);

   rc = munmap(dest_ptr, statbuf.st_size);
   assert(rc == 0);
   rc = munmap(src_ptr, statbuf.st_size);
   assert(rc == 0);

   return 0;
}
