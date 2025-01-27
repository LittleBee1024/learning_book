#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/io.h>

#define PORT_FILE "/dev/mem"

char *prgname;

static int write_one(unsigned int mem, unsigned int val, int size)
{
   static int fd = -1;
   unsigned char b;
   unsigned short w;

   if (fd < 0)
      fd = open(PORT_FILE, O_WRONLY);
   if (fd < 0)
   {
      fprintf(stderr, "%s: %s: %s\n", prgname, PORT_FILE, strerror(errno));
      return 1;
   }
   lseek(fd, mem, SEEK_SET);

   if (size == 4)
   {
      write(fd, &val, 4);
   }
   else if (size == 2)
   {
      w = val;
      write(fd, &w, 2);
   }
   else
   {
      b = val;
      write(fd, &b, 1);
   }
   return 0;
}

int main(int argc, char **argv)
{
   if (argc != 4)
   {
      fprintf(stderr, "Please input correct arguments: %s <mem> <size> <val>\n", argv[0]);
      exit(-1);
   }
   prgname = argv[0];
   unsigned int mem = strtol(argv[1], NULL, 16);
   int size = atoi(argv[2]);
   assert(size == 1 || size == 2 || size == 4);
   unsigned int val = strtol(argv[3], NULL, 16);


   printf("write_one: mem=0x%x, val=0x%x, size=%d\n", mem, val, size);
   int rc = write_one(mem, val, size);
   assert(rc == 0);

   return 0;
}
