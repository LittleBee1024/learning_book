#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

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
   sleep(1); // wait for read signal IO
}

static int read_fd = -1;
static void signalio_handler(int signum)
{
   printf("[Read Process - Signal_IO] Receive a signal from global FIFO, signalnum:%d\n", signum);
   assert(read_fd > 0);
   char buf[1024];
   int n = read(read_fd, buf, sizeof(data));
   printf("[Read Process - Signal_IO] Read %d bytes from the device: %s\n", n, buf);
   close(read_fd);
}

void block_read()
{
   printf("[Read Process - Main] Start\n");

   read_fd = open(GFIFO_DEV, O_RDWR);
   assert(read_fd > 0);

   signal(SIGIO, signalio_handler);
   fcntl(read_fd, F_SETOWN, getpid());
   int oflags = fcntl(read_fd, F_GETFL);
   fcntl(read_fd, F_SETFL, oflags | FASYNC);

   printf("[Read Process - Main] End\n");
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

