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
