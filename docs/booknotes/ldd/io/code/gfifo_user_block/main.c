#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>

#define GFIFO_DEV "/dev/gfifo"
const char data[] = "Hello, global gfifo\n";

void sleep_write()
{
   printf("[Write Process] Start\n");

   int fd = open(GFIFO_DEV, O_RDWR);
   assert(fd > 0);

   sleep(1);
   printf("[Write Process] Start to write after sleep\n");
   int n = write(fd, data, sizeof(data));
   printf("[Write Process] Written %d bytes to the device\n", n);

   close(fd);

   printf("[Write Process] End\n");
}

void block_read()
{
   printf("[Read Process] Start\n");

   int fd = open(GFIFO_DEV, O_RDWR);
   assert(fd > 0);

   char buf[1024];
   int n = read(fd, buf, sizeof(data));
   printf("[Read Process] Read %d bytes from the device: %s\n", n, buf);

   close(fd);

   printf("[Read Process] End\n");
}

int main(int argc, char **argv)
{
   pid_t pid = fork();
   if (pid == 0)
   {
      // child process
      sleep_write();
      return 0;
   }

   // parent process
   block_read();

   int rc = wait(NULL);
   assert(rc != -1);

   return 0;
}

