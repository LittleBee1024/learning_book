#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <sys/io.h>

#define PORT_FILE "/dev/port"

char *prgname;

static int read_and_print_one_i386(unsigned int port, int size)
{
   static int iopldone = 0;

   if (port > 1024)
   {
      if (!iopldone && iopl(3))
      {
         fprintf(stderr, "%s: iopl(): %s\n", prgname, strerror(errno));
         return 1;
      }
      iopldone++;
   }
   else if (ioperm(port, size, 1))
   {
      fprintf(stderr, "%s: ioperm(%x): %s\n", prgname,
              port, strerror(errno));
      return 1;
   }

   if (size == 4)
      printf("%04x: 0x%08x\n", port, inl(port));
   else if (size == 2)
      printf("%04x: 0x%04x\n", port, inw(port));
   else
      printf("%04x: 0x%02x\n", port, inb(port));
   return 0;
}

static int read_and_print_one(unsigned int port, int size)
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
   lseek(fd, port, SEEK_SET);

   if (size == 4)
   {
      read(fd, &l, 4);
      printf("%04x: 0x%08x\n", port, l);
   }
   else if (size == 2)
   {
      read(fd, &w, 2);
      printf("%04x: 0x%04x\n", port, w & 0xffff);
   }
   else
   {
      read(fd, &b, 1);
      printf("%04x: 0x%02x\n", port, b & 0xff);
   }
   return 0;
}


int main(int argc, char **argv)
{
   if (argc != 3)
   {
      fprintf(stderr, "Please input correct arguments: %s <port> <size>\n", argv[0]);
      exit(-1);
   }
   prgname = argv[0];
   unsigned int port = strtol(argv[1], NULL, 16);
   int size = atoi(argv[2]);
   assert(size == 1 || size == 2 || size == 4);

   printf("read_and_print_one: port=0x%x, size=%d\n", port, size);
   int rc = read_and_print_one(port, size);
   assert(rc == 0);

   printf("read_and_print_one_i386: port=0x%x, size=%d\n", port, size);
   rc = read_and_print_one_i386(port, size);
   assert(rc == 0);

   return 0;
}
