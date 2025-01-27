#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define ASSERT(A, M, ...)                                                                  \
   if (!(A))                                                                               \
   {                                                                                       \
      fprintf(stderr, "[ERROR] (%s:%d:%s - %s): " M "\n",                                   \
              __FILE__, __LINE__, __func__, errno ? strerror(errno) : "0", ##__VA_ARGS__); \
      exit(EXIT_FAILURE);                                                                  \
   }

#define GMEM0_DEV "/dev/gmem0"

const char data[] = "Hello, global memory\n";
#define MEM_CLEAR 0x1

int main()
{
   int fd = open(GMEM0_DEV, O_RDWR);
   ASSERT(fd > -1, "Failed to open %s", GMEM0_DEV);

   int nBytes = write(fd, data, sizeof(data));
   printf("Written %d bytes to the device\n", nBytes);

   int pos = lseek(fd, 0, SEEK_CUR);
   printf("Current device position is %d\n", pos);
   pos = lseek(fd, 0, SEEK_SET);
   printf("Set device position to %d\n", pos);

   char buf[1024];
   int rc = read(fd, buf, nBytes);
   printf("Read %d bytes from the device: %s\n", rc, buf);

   rc = ioctl(fd, MEM_CLEAR);
   ASSERT(rc == 0, "Failed to clear memory");

   close(fd);
   return 0;
}
