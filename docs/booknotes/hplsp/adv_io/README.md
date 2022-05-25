# 高级IO函数

> [《Linux高性能服务器编程》 - 游双 ](https://1drv.ms/b/s!AkcJSyT7tq80c1DmkdcxK7oScvQ)第6章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/adv_io/code)中找到

## pipe函数

### pipe
```cpp
#include <unistd.h>

// 打开一个管道
//  fd[0] - 管道读端
//  fd[1] - 管道写端
//  如果管道中没有数据，读操作阻塞
//  如果所有写端被关闭，读操作返回零
//  如果所有读端被关闭，写操作失败，并触发`SIGPIPE`信号
int pipe(int fd[2]);
```
![adv_io_pipe](./images/adv_io_pipe.png)

[例子"pipe"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/adv_io/code/pipe)，实现了子进程同步读取父进程往管道中写入的数据：

```cpp hl_lines="9 11 24 33 35 36"
#define MSGSIZE 16
const char *msg1 = "hello, world #1";
const char *msg2 = "hello, world #2";

void write_data(int write_fd)
{
   printf("Parent Task Starts\n");

   int nbytes = write(write_fd, msg1, MSGSIZE);
   printf("parent process wrote %d bytes to child process: %s\n", nbytes, msg1);
   nbytes = write(write_fd, msg2, MSGSIZE);
   printf("parent process wrote %d bytes to child process: %s\n", nbytes, msg2);

   printf("Parent Task Ends\n");
}

void read_data(int read_fd)
{
   printf("Child Task Starts\n");

   char inbuf[MSGSIZE];
   int nbytes = 0;

   while ((nbytes = read(read_fd, inbuf, MSGSIZE)) > 0)
      printf("child process read %d bytes to from parent process: %s\n", nbytes, inbuf);

   printf("Child Task Ends\n");
}

int main(int argc, char **argv)
{
   int fd[2];
   pipe(fd);

   int read_fd = fd[0];
   int write_fd = fd[1];

   pid_t pid = fork();
   if (pid == 0)
   {
      close(write_fd);
      read_data(read_fd);
      close(read_fd);
      return 0;
   }

   close(read_fd);
   sleep(1);
   write_data(write_fd);
   close(write_fd);

   wait(NULL);
   return 0;
}
```
```bash
> ./main 
Child Task Starts
Parent Task Starts
parent process wrote 16 bytes to child process: hello, world #1
parent process wrote 16 bytes to child process: hello, world #2
Parent Task Ends
child process read 16 bytes to from parent process: hello, world #1
child process read 16 bytes to from parent process: hello, world #2
Child Task Ends
```

### socketpair
```cpp
#include <sys/socket.h>

// 创建双向管道
//  domain - 只能是UNIX本地协议族AF_UNIX
//  type, protocol - 本地协议族的参数
//  fd[2] - 管道的两端，都可读可写，一端写，必须从另一端读，反之亦然。
//  如果管道中没有数据，读操作阻塞
//  如果管道一端的文件描述符都被关闭，则另一端的读操作返回零
int socketpair(int domain, int type, int protocol, int fd[2]);
```
![adv_io_socketpair](./images/adv_io_socketpair.png)

[例子"socketpair"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/adv_io/code/socketpair)，实现了在两个进程间传递数据：

```cpp hl_lines="10 13 23 27 36"
#define MSGSIZE 16
const char *msg1 = "hello, world #1";
const char *msg2 = "hello, world #2";

void parent_rw(int socket)
{
   printf("[Parent] Task Starts\n");

   char inbuf[MSGSIZE];
   int nbytes = read(socket, inbuf, MSGSIZE);
   printf("[Parent] read %d bytes to from child process: %s\n", nbytes, inbuf);

   nbytes = write(socket, msg1, MSGSIZE);
   printf("[Parent] wrote %d bytes to child process: %s\n", nbytes, msg1);

   printf("[Parent] Task Ends\n");
}

void child_wr(int socket)
{
   printf("[Child] Task Starts\n");

   int nbytes = write(socket, msg2, MSGSIZE);
   printf("[Child] wrote %d bytes to parent process: %s\n", nbytes, msg2);

   char inbuf[MSGSIZE];
   nbytes = read(socket, inbuf, MSGSIZE);
   printf("[Child] read %d bytes to from parent process: %s\n", nbytes, inbuf);

   printf("[Child] Task Ends\n");
}

int main()
{
   int fd[2];
   socketpair(AF_UNIX, SOCK_STREAM, 0, fd);

   const int parentSocket = 0;
   const int childSocket = 1;

   pid_t pid = fork();
   if (pid == 0)
   {
      close(fd[parentSocket]);
      child_wr(fd[childSocket]);
      close(fd[childSocket]);
      return 0;
   }

   close(fd[childSocket]);
   sleep(1);
   parent_rw(fd[parentSocket]);
   close(fd[parentSocket]);

   wait(NULL);
   return 0;
}
```
```bash
> ./main 
[Child] Task Starts
[Child] wrote 16 bytes to parent process: hello, world #2
[Parent] Task Starts
[Parent] read 16 bytes to from child process: hello, world #2
[Parent] wrote 16 bytes to child process: hello, world #1
[Parent] Task Ends
[Child] read 16 bytes to from parent process: hello, world #1
[Child] Task Ends
```

### mkfifo
```cpp
#include <sys/stat.h>

// 创建命名管道
//  pathname - 命名管道文件名，需要保证在创建前不存在此文件
//  mode - 文件属性
//  需要同时以“只读”和“只写”的方式打开命名管道，并在两边进行同步读写
int mkfifo(const char *pathname, mode_t mode);
```
![adv_io_mkfifo](./images/adv_io_mkfifo.png)

[例子"mkfifo"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/adv_io/code/mkfifo)，创建了一个`/tmp/fifo`命名管道，并在两个进程中通过此管道同步数据：

```cpp hl_lines="10 12 16 17 28 29 33 35 44"
#define NAMED_FIFO "/tmp/fifo"
#define MSGSIZE 16
const char *msg1 = "hello, world #1";
const char *msg2 = "hello, world #2";

void parent_rw()
{
   printf("[Parent] Task Starts\n");

   int fd_read = open(NAMED_FIFO, O_RDONLY);
   char inbuf[MSGSIZE];
   int nbytes = read(fd_read, inbuf, MSGSIZE);
   printf("[Parent] read %d bytes to from child process: %s\n", nbytes, inbuf);
   close(fd_read);

   int fd_write = open(NAMED_FIFO, O_WRONLY);
   nbytes = write(fd_write, msg1, MSGSIZE);
   printf("[Parent] wrote %d bytes to child process: %s\n", nbytes, msg1);
   close(fd_write);

   printf("[Parent] Task Ends\n");
}

void child_wr()
{
   printf("[Child] Task Starts\n");

   int fd_write = open(NAMED_FIFO, O_WRONLY);
   int nbytes = write(fd_write, msg2, MSGSIZE);
   printf("[Child] wrote %d bytes to parent process: %s\n", nbytes, msg2);
   close(fd_write);

   int fd_read = open(NAMED_FIFO, O_RDONLY);
   char inbuf[MSGSIZE];
   nbytes = read(fd_read, inbuf, MSGSIZE);
   printf("[Child] read %d bytes to from parent process: %s\n", nbytes, inbuf);
   close(fd_read);

   printf("[Child] Task Ends\n");
}

int main(int argc, char **argv)
{
   int rc = mkfifo(NAMED_FIFO, 0777);
   if (rc == -1)
   {
      perror(NAMED_FIFO);
      return -1;
   }

   pid_t pid = fork();
   if (pid == 0)
   {
      child_wr();
      return 0;
   }

   sleep(1);
   parent_rw();

   wait(NULL);
   unlink(NAMED_FIFO);
   return 0;
}
```
```bash
> ./main 
[Child] Task Starts
[Parent] Task Starts
[Child] wrote 16 bytes to parent process: hello, world #2
[Parent] read 16 bytes to from child process: hello, world #2
[Parent] wrote 16 bytes to child process: hello, world #1
[Child] read 16 bytes to from parent process: hello, world #1
[Child] Task Ends
[Parent] Task Ends
```

## dup函数

### dup
```cpp
#include <unistd.h>

// 复制文件描述符，新的文件描述符的值由系统指定
//  oldfd - 要复制的文件描述符
int dup(int oldfd);
```
[例子"dup"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/adv_io/code/dup)，复制了标准输出，因此有两个文件描述符可以输出信息：
```cpp hl_lines="7 9 10"
const char msg1[] = "hello, world #1\n";
const char msg2[] = "hello, world #2\n";

int main()
{
   int stdout_fd = 1;
   int stdout_fd_copy = dup(stdout_fd);

   printf("%s", msg1);
   write(stdout_fd_copy, msg2, sizeof(msg2) - 1); // print to stdout

   close(stdout_fd_copy);
   return 0;
}
```
```bash
> ./main 
hello, world #1
hello, world #2
```

### dup2
```cpp
#include <unistd.h>

// 复制文件描述符，新的文件描述符由用户传入
//  oldfd - 要复制的文件描述符
//  newfd - 新的文件描述符的值，如果有被此文件描述符打开的文件，会被关闭
int dup2(int oldfd, int newfd);
```
[例子"dup2"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/adv_io/code/dup2)，将标准输出重定位到了文件：
```cpp hl_lines="8 10 12"
const char msg1[] = "hello, world #1\n";
const char msg2[] = "hello, world #2\n";

int main()
{
   int fd = open("dup2.log", O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   int stdout_fd = 1;
   int rc = dup2(fd, stdout_fd);

   printf("%s", msg1); // print to log file because the fd is redirected
   fflush(stdout); // no flush will lose the data
   write(stdout_fd, msg2, sizeof(msg2) - 1);

   close(stdout_fd);
   close(fd);

   return 0;
}
```
```bash
> cat dup2.log
hello, world #1
hello, world #2
```

## mmap函数

## sendfile函数

## splice函数

## tee函数

