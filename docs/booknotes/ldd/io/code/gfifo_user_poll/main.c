#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <string.h>

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
   struct pollfd fds[1];
   memset(fds, 0, sizeof(fds));
   fds[0].fd = fd;
   fds[0].events = POLLIN;
   int nfds = 1;
   int timeout = 1 * 1000; // milliseconds
   while (1)
   {
      int rc = poll(fds, nfds, timeout);
      assert(rc >= 0);
      if (rc == 0)
      {
         printf("[Poll Process] poll() timeout, no data to read\n");
         continue;
      }

      if (fds[0].revents & POLLIN)
      {
         int n = read(fd, buf, sizeof(data));
         printf("[Poll Process] Read %d bytes from the device: %s\n", n, buf);
         break;
      }
      printf("[Poll Process] Receive unexpected event 0x%x\n", fds[0].revents);
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
