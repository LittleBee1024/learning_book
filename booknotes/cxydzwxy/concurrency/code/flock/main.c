#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <assert.h>

#define LOCK_FILE "lock_file.lock"

void child_start()
{
   int fd = open(LOCK_FILE, O_RDONLY);
   int rc = flock(fd, LOCK_EX);
   assert(rc == 0);

   printf("Child process has started\n");
   sleep(1);
   printf("Child process has finished\n");

   rc = flock(fd, LOCK_UN);
   assert(rc == 0);
   close(fd);
}

void parent_start()
{
   int fd = open(LOCK_FILE, O_RDONLY);
   int rc = flock(fd, LOCK_EX);
   assert(rc == 0);

   printf("Parent process has started\n");
   sleep(1);
   printf("Parent process has finished\n");

   rc = flock(fd, LOCK_UN);
   assert(rc == 0);
   close(fd);
}

int main(void)
{
   int fd = open(LOCK_FILE, O_RDONLY | O_CREAT, 0666);
   assert(fd > 0);
   close(fd);

   pid_t pid = fork();
   if (pid == 0)
   {
      child_start();
      return 0;
   }

   parent_start();
   wait(NULL);

   return 0;
}
