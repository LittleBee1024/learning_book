# 计算机漫游

> [《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG)，第一章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/01/code)中找到

## 什么是信息
系统中所有的信息都是由一串比特表示的。例如，一个简单的["hello world"](./code/hello/main.c)程序，在计算机中被存储为一串数字。计算机在不同的上下文中，对同样的一串数字可能表示为一个整数、浮点数、字符串或者机器指令。
```bash
> hd main.c
00000000  23 69 6e 63 6c 75 64 65  20 3c 73 74 64 69 6f 2e  |#include <stdio.|
00000010  68 3e 0a 0a 69 6e 74 20  6d 61 69 6e 28 29 0a 7b  |h>..int main().{|
00000020  0a 20 20 20 70 72 69 6e  74 66 28 22 68 65 6c 6c  |.   printf("hell|
00000030  6f 2c 20 77 6f 72 6c 64  5c 6e 22 29 3b 0a 20 20  |o, world\n");.  |
00000040  20 72 65 74 75 72 6e 20  30 3b 0a 7d 0a           | return 0;.}.|
0000004d
```

因此，计算机中的信息，可以理解为**位+上下文**。当我们解释某个信息时，不仅要知道它是什么，还要知道它处于什么环境。

## 如何让程序跑起来
以["hello world"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/01/code/hello/main.c)程序为例，计算机是如何通过一串数字，在屏幕上显式"hello, world"字样的？

### 生成可执行文件

![compile_link](./images/compile_link.png)

上图显式了从源文件"hello.c"到可执行文件"hello"的过程，主要包括：

* 预处理阶段
* 编译阶段
* 汇编阶段
* 链接阶段

更详细的过程，可参考博文[《程序员的自我修养-编译》](../../cxydzwxy/compile/README.md)。

### 运行可执行文件

