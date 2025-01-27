#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <sys/user.h>
#include <sys/ioctl.h>

#define GMEMP_DEV "/dev/gmemp"
#define MEM_CLEAR 0x1
const char data[] = "Hello, global gmemp\n";

void normal_write()
{
   printf("[Write Process] Start\n");

   int fd = open(GMEMP_DEV, O_RDWR);
   assert(fd > 0);

   int n = write(fd, data, sizeof(data));
   printf("[Write Process] Written %d bytes to the device\n", n);

   close(fd);

   printf("[Write Process] End\n");
}

void mmap_read()
{
   printf("[Read Process] Start\n");

   int fd = open(GMEMP_DEV, O_RDWR);
   assert(fd > 0);

   const size_t MMAP_SIZE = sizeof(data);
   void *ptr = mmap(NULL, MMAP_SIZE, PROT_READ, MAP_SHARED, fd, 0);
   assert(ptr != MAP_FAILED);

   char buf[MMAP_SIZE + 1];
   buf[MMAP_SIZE] = 0;
   memcpy(buf, ptr, MMAP_SIZE);
   printf("[Read Process] Read %zu bytes from the device: %s\n", MMAP_SIZE, buf);

   int rc = munmap(ptr, MMAP_SIZE);
   assert(rc == 0);
   close(fd);

   printf("[Read Process] End\n");
}

void clear_gfifo()
{
   int fd = open(GMEMP_DEV, O_RDWR);
   assert(fd > 0);
   int rc = ioctl(fd, MEM_CLEAR);
   assert(rc == 0);
   close(fd);
}

int main(int argc, char **argv)
{
   pid_t pid = fork();
   if (pid == 0)
   {
      // child process
      normal_write();
      return 0;
   }

   // parent process
   int rc = wait(NULL);
   assert(rc != -1);
   mmap_read();

   clear_gfifo();

   return 0;
}

