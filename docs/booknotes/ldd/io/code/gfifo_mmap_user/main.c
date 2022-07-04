#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <sys/user.h>

#define GFIFOP_DEV "/dev/gfifop"
#define GFIFOP_MMAP_SIZE (PAGE_SIZE * 1)
const char data[] = "Hello, global gfifop\n";

void mmap_write()
{
   printf("[Write Process] Start\n");

   int fd = open(GFIFOP_DEV, O_RDWR);
   assert(fd > 0);

   void *ptr = mmap(NULL, GFIFOP_MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   assert(ptr != MAP_FAILED);

   char data[GFIFOP_MMAP_SIZE];
   for (int i = 0; i < GFIFOP_MMAP_SIZE; i++)
      data[i] = 'a' + (char)(i % 26);

   memcpy(ptr, data, GFIFOP_MMAP_SIZE);
   printf("[Write Process] Written %zu bytes to the device\n", GFIFOP_MMAP_SIZE);

   int rc = munmap(ptr, GFIFOP_MMAP_SIZE);
   assert(rc == 0);
   close(fd);

   printf("[Write Process] End\n");
}

void mmap_read()
{
   printf("[Read Process] Start\n");

   int fd = open(GFIFOP_DEV, O_RDWR);
   assert(fd > 0);

   const size_t read_size = 26;
   void *ptr = mmap(NULL, read_size, PROT_READ, MAP_SHARED, fd, 0);
   assert(ptr != MAP_FAILED);

   char buf[read_size + 1];
   buf[read_size] = 0;
   memcpy(buf, ptr, read_size);
   printf("[Read Process] Read %zu bytes from the device: %s\n", read_size, buf);

   int rc = munmap(ptr, read_size);
   assert(rc == 0);
   close(fd);

   printf("[Read Process] End\n");
}

int main(int argc, char **argv)
{
   pid_t pid = fork();
   if (pid == 0)
   {
      // child process
      mmap_write();
      return 0;
   }

   // parent process
   int rc = wait(NULL);
   assert(rc != -1);
   mmap_read();

   return 0;
}

