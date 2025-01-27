#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <string.h>

#define MAX_EVENTS 5
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
   int epoll_fd = epoll_create1(0);
   assert(epoll_fd > 0);
   struct epoll_event event, events[MAX_EVENTS];
   event.events = EPOLLIN;
   event.data.fd = fd;
   int rc = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
   assert(rc != -1);
   int timeout = 1 * 1000; // milliseconds
   while (1)
   {
      int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
      assert(event_count >= 0);
      if (event_count == 0)
      {
         printf("[Poll Process] epoll() timeout, no data to read\n");
         continue;
      }

      assert(event_count == 1);
      if (events[0].events & EPOLLIN)
      {
         int n = read(events[0].data.fd, buf, sizeof(data));
         printf("[Poll Process] Read %d bytes from the device: %s\n", n, buf);
         break;
      }
      printf("[Poll Process] Receive unexpected event 0x%x\n", events[0].events);
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
