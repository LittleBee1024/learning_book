#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define MAX_LEN 100
void input_handler(int num)
{
   char data[MAX_LEN];
   int len;

   len = read(STDIN_FILENO, &data, MAX_LEN);
   data[len] = 0;
   printf("input available:%s\n", data);
}

int main()
{
   int oflags;

   // 1. 连接信号和信号处理函数
   signal(SIGIO, input_handler);
   // 2. 通过F_SETOWN设置设备文件的拥有者为本进程，这样设备驱动发出的信号才能被本进程接收
   fcntl(STDIN_FILENO, F_SETOWN, getpid());
   // 3. 通过FASYNC，设置异步通知模式，每当FASYNC标志改变时，驱动程序中的`fasync()`函数被执行
   oflags = fcntl(STDIN_FILENO, F_GETFL);
   fcntl(STDIN_FILENO, F_SETFL, oflags | FASYNC);

   while (1);
   return 0;
}