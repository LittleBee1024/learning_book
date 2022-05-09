#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <assert.h>

// lock file file description
int fd;

void child_start()
{
   int rc = flock(fd, LOCK_EX);
   assert(rc == 0);

   printf("Child process has started\n");
   sleep(1);
   printf("Child process has finished\n");

   rc = flock(fd, LOCK_UN);
   assert(rc == 0);
}

void parent_start()
{
   int rc = flock(fd, LOCK_EX);
   assert(rc == 0);

   printf("Parent process has started\n");
   sleep(1);
   printf("Parent process has finished\n");

   rc = flock(fd, LOCK_UN);
   assert(rc == 0);
}

int main(void)
{
   fd = open("lock_file.lock", O_RDWR | O_CREAT, 0666);
   if (fd <= 0)
      perror("open");
   assert(fd > 0);

   pid_t pid = fork();
   if (pid == 0)
   {
      child_start();
      return 0;
   }

   parent_start();
   wait(NULL);

   close(fd);

   return 0;
}
