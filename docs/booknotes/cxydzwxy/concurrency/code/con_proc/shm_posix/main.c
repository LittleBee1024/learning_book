#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

// Refer to https://www.prodevelopertutorial.com/linux-system-programming-posix-shared-memory/

#define FILE_PATH "/my_shm_test"
#define FILE_SIZE 512

const char buf[] = "Hello World";

void read_data()
{
   int fd = shm_open(FILE_PATH, O_RDONLY, S_IRUSR | S_IWUSR);
   if (fd == -1)
   {
      perror("shm_open");
      return;
   }

   struct stat st;
   if (fstat(fd, &st))
   {
      printf("\nfstat error: [%s]\n", strerror(errno));
      close(fd);
      return;
   }
   const size_t shm_size = st.st_size;
   printf("The size of shard memory is %zu bytes\n", shm_size);

   // map shared memory
   void *addr = mmap(NULL, shm_size, PROT_READ, MAP_SHARED, fd, 0);

   // place data into memory
   char data[shm_size];
   memcpy(data, addr, shm_size);

   printf("Read from shared memory: \"%s\"\n", data);

   // mmap cleanup
   int res = munmap(addr, FILE_SIZE);
   assert(res != -1);

   // shm_open cleanup
   res = shm_unlink(FILE_PATH);
   assert(res != -1);
}

int main(int argc, char *argv[])
{
   // get shared memory file descriptor
   int fd = shm_open(FILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
   if (fd == -1)
   {
      perror("shm_open");
      return 1;
   }

   // increase the size
   int res = ftruncate(fd, FILE_SIZE);

   // map shared memory
   void *addr = mmap(NULL, FILE_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);

   // place data into memory
   memcpy(addr, buf, sizeof(buf));

   pid_t pid = fork();
   if (pid == 0)
   {
      read_data();
      return 0;
   }

   assert(pid > 0);
   wait(NULL);

   // mmap cleanup
   printf("munmap in parent process\n");
   res = munmap(addr, FILE_SIZE);
   assert(res != -1);

   // unlink in child process, no need unlink here, munmap is enough
   // shm_unlink(FILE_PATH);

   return 0;
}
