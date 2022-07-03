#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/io.h>

#define PORT_FILE "/dev/port"

char *prgname;

static int write_one_i386(unsigned int port, unsigned int val, int size)
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
      outl(val, port);
   else if (size == 2)
      outw(val & 0xffff, port);
   else
      outb(val & 0xff, port);
   return 0;
}

static int write_one(unsigned int port, unsigned int val, int size)
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
   lseek(fd, port, SEEK_SET);

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
      fprintf(stderr, "Please input correct arguments: %s <port> <size> <val>\n", argv[0]);
      exit(-1);
   }
   prgname = argv[0];
   unsigned int port = strtol(argv[1], NULL, 16);
   int size = atoi(argv[2]);
   assert(size == 1 || size == 2 || size == 4);
   unsigned int val = strtol(argv[3], NULL, 16);


   printf("write_one: port=0x%x, val=0x%x, size=%d\n", port, val, size);
   int rc = write_one(port, val, size);
   assert(rc == 0);

   printf("write_one_i386: port=0x%x, val=0x%x, size=%d\n", port, val, size);
   rc = write_one_i386(port, val, size);
   assert(rc == 0);

   return 0;
}
