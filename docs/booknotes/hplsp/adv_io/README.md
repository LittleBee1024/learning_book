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
### mkfifo

## dup/dup2函数

## mmap函数

## sendfile函数

## splice函数

## tee函数

