#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#define GFIFO_DEV "/dev/gfifo"
const char data[] = "Hello, global gfifo\n";

void sleep_write()
{
   printf("[Write Process] Start\n");

   int fd = open(GFIFO_DEV, O_RDWR);
   assert(fd > 0);

   sleep(5);
   int n = write(fd, data, sizeof(data));
   printf("[Write Process] Written %d bytes to the device\n", n);

   close(fd);

   printf("[Write Process] End\n");
}

void poll_read()
{
   printf("[Poll Process] Start\n");

   int fd = open(GFIFO_DEV, O_RDONLY | O_NONBLOCK);
   assert(fd > 0);

   char buf[1024];
   fd_set rfds;
   struct timeval timeout;
   while (1)
   {
      FD_ZERO(&rfds);
      FD_SET(fd, &rfds);
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      int rc = select(fd + 1, &rfds, NULL, NULL, &timeout);
      assert(rc >= 0);
      if (rc == 0)
      {
         printf("[Poll Process] select() timeout, no data to read\n");
         continue;
      }

      if (FD_ISSET(fd, &rfds))
      {
         int n = read(fd, buf, sizeof(data));
         printf("[Poll Process] Read %d bytes from the device: %s\n", n, buf);
         break;
      }
   }

   close(fd);
   printf("[Poll Process] End\n");
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
   poll_read();

   int rc = wait(NULL);
   assert(rc != -1);

   return 0;
}
