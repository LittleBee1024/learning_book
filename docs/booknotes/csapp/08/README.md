# 异常流控制

> [《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG)，第八章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/08/code)中找到


异常控制流(Exceptional Control Flow, ECF)表示由非常规程序指令(`ret`，`call`等)触发的控制权跳转，其发生在计算机系统的各个层次：

* 硬件层 - 异常
    * 硬件检测的事件会触发控制突然转移到异常处理程序
* 操作系统层 - 系统调用
    * 内核通过上下文切换将控制从一个用户进程转移到另一个用户进程
* 应用层 - 信号
    * 信号接收进程接收到信号后，会将控制突然转移到它的一个信号处理程序

## 异常

异常是控制流中的突变，一部分由硬件实现，一部分由操作系统实现。异常发生的过程如下：

* 处理器检测到有事件发生
* 根据事件种类，从**异常表**中选择对应的异常处理程序地址，并进行跳转
* 当异常处理程序完成后，根据异常类型，会发生以下3种情况：
    * 将控制返回给当前指令，即当事件发生时正在执行的指令
    * 将控制返回给下一条指令，即没有发生异常将会执行的下一条指令
    * 终止被终端的程序

异常跳转的过程类似于过程调用，但也有一些不同之处：

* 异常跳转的返回地址会根据异常类型不同而不同，可能是当前指令，也可能是下一条指令
* 处理器也会保存现场，但如果控制是从用户程序转移到内核，所有这些项目都被压到内核栈，而不是压到用户栈

### 异常的类别

| 类别 | 原因 | 异步/同步 | 返回行为 |
| --- | --- | --- | --- |
| 中断 | 来自I/O设备的信号 | 异步 | 总是返回到下一条指令 |
| 陷阱 | 有意的异常，如系统调用 | 同步 | 总是返回到下一条指令 |
| 故障 | 潜在可恢复的错误 | 同步 | 可能返回到当前指令 |
| 终止 | 不可恢复的错误 | 同步 | 不会返回 |

* 异步/同步
    * 中断是来自处理器外部的I/O设备的信号的结果，是异步的
    * 除了中断，剩下的异常类型都是同步发生的，是执行当前指令的结果
* 终止/返回
    * “中断”和“陷阱”总会返回到下一条指令
    * “故障”可能返回到当前指令，也可能终止程序
    * “终止”异常肯定会终止程序

![exception_handling](./images/exception_handling.png)

### Linux故障
* 除法错误
    * 当试图除以零，发生除法错误，Linux报告为“浮点异常(Floating exception)”，并终止程序
* 一般保护故障
    * 当引用了一个未定义的虚拟内存区域，或者试图写一个只读段，Linux报告为“段故障(Segmentation fault)”，并终止程序
* 缺页
    * 当发生缺页故障，异常处理程序将虚拟内存的一个页面映射到物理内存的一个页面，然后重新执行这条产生故障的指令

[例子"div_zero_fault"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/08/code/div_zero_fault)自定义了“除法错误”的处理程序。类似地，[例子"seg_fault"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/08/code/seg_fault)自定义了“段故障”的处理程序。故障的异常处理程序会返回到当前指令，因此只要异常处理程序不主动终止程序，会不断被触发。

```cpp title="Divide by Zero Fault" hl_lines="13 19 24"
void sig_handler(int signo)
{
    static int num = 0;
    if (signo == SIGFPE)
    {
        printf("Divide by zero fault happens %d times\n", num++);
    }

    const int SIGFPE_NUM = 5;
    if (num == SIGFPE_NUM)
    {
        printf("Abort the program after %d divide by zero fault\n", num);
        std::abort();
    }
}

int main()
{
    signal(SIGFPE, sig_handler);

    printf("Trigger divide by zero fault\n");
    int a = 1;
    int b = 0;
    a = a / b;

    return 0;
}
```
```bash
> ./main
Trigger divide by zero fault
Divide by zero fault happens 0 times
Divide by zero fault happens 1 times
Divide by zero fault happens 2 times
Divide by zero fault happens 3 times
Divide by zero fault happens 4 times
Abort the program after 5 divide by zero fault
Aborted (core dumped)
```

### Linux陷阱(系统调用)

Linux提供了几百种系统调用，每个系统调用都有一个唯一的整数号，对应于一个到内核中的偏移量（注意：这个跳转表和异常表不一样）。在x86-64系统上，系统调用是通过一条称为`syscall`的陷阱指令来提供的。所有系统调用的参数都是通过通用寄存器而不是栈传递的。

[例子"write_syscall"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/08/code/write_syscall)用4种方法在终端打印“hello world”字符串。其中，汇编代码实现的`asm_write`函数展示了`syscall`指令的工作情况：
```cpp hl_lines="11"
int asm_write(int fd, const void* buffer, size_t size)
{
   int ret = 0;

   // write(rdi=fd, rsi=buffer, rdx=size);
   //   write system call ID = 1 (rax)
   asm("movl %1,%%edi \n\t"
       "movq %2,%%rsi \n\t"
       "movq %3,%%rdx \n\t"
       "movq $1,%%rax \n\t"
       "syscall       \n\t"
       "movl %%eax,%0 \n\t"
       :"=rm"(ret)
       :"rm"(fd),"rm"(buffer),"rm"(size));

   return ret;
}

int main()
{
   printf("hello, world0\n");
   write(1/*fileno(stdout)*/, "hello, world1\n", 14);
   syscall(SYS_write, 1, "hello, world2\n", 14);
   asm_write(1, "hello, world3\n", 14);

   return 0;
}
```
```bash
> ./main
hello, world0
hello, world1
hello, world2
hello, world3
```


