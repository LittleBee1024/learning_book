#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <sys/io.h>

#define PORT_FILE "/dev/mem"

char *prgname;

static int read_and_print_one(unsigned int mem, int size)
{
   static int fd = -1;
   unsigned char b;
   unsigned short w;
   unsigned int l;

   if (fd < 0)
      fd = open(PORT_FILE, O_RDONLY);
   if (fd < 0)
   {
      fprintf(stderr, "%s: %s: %s\n", prgname, PORT_FILE, strerror(errno));
      return 1;
   }
   lseek(fd, mem, SEEK_SET);

   if (size == 4)
   {
      read(fd, &l, 4);
      printf("%04x: 0x%08x\n", mem, l);
   }
   else if (size == 2)
   {
      read(fd, &w, 2);
      printf("%04x: 0x%04x\n", mem, w & 0xffff);
   }
   else
   {
      read(fd, &b, 1);
      printf("%04x: 0x%02x\n", mem, b & 0xff);
   }
   return 0;
}


int main(int argc, char **argv)
{
   if (argc != 3)
   {
      fprintf(stderr, "Please input correct arguments: %s <mem> <size>\n", argv[0]);
      exit(-1);
   }
   prgname = argv[0];
   unsigned int mem = strtol(argv[1], NULL, 16);
   int size = atoi(argv[2]);
   assert(size == 1 || size == 2 || size == 4);

   printf("read_and_print_one: mem=0x%x, size=%d\n", mem, size);
   int rc = read_and_print_one(mem, size);
   assert(rc == 0);

   return 0;
}
