# GDB

> GDB的高级用法，所有样例可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/debug_hacks/gdb/code)中找到

## 简介
GDB用于程序调试，常见的操作有：

* 启动调试程序，并可附加启动参数
* 根据指定条件，暂停程序的运行
* 查看暂停处程序运行的各种状态
* 按需要修改程序的状态后，继续运行程序以观察状态的变化

> 可通过["GDB的用户手册"](https://sourceware.org/gdb/current/onlinedocs/gdb/Command-and-Variable-Index.html#Command-and-Variable-Index)查看GDB所有命令的用法

## 启动与执行

### 启动进程
* `gdb <EXE>`
* `gdb --args <EXE> <args...>`

### 挂载到运行的进程
* `gdb -p <PID>`

### 执行
* `continue <次数>`，达到指定次数前，执行到断点时不暂停
* `finish`，执行完当前函数后暂停
* `until`，执行完当前代码块后暂停，如果是循环，执行完循环后暂停

## 设置断点

### 普通断点(break point)
* `break <函数名>`
* `break <文件名:行号>`
* `break <*地址>`
* `break <+/-偏移量>`

### 条件断点(conditional break point)
* `break <断点> if <条件>`

### 监视点(watch point)
* `watch <变量/地址>`，监视变量或地址，当发生变化时暂停程序

### 捕获点(catch point)
* `catch throw`，捕获C++程序的异常，当发生异常时，暂停程序
    * [实例代码](./code/catch_throw/main.cpp)被GDB捕获了异常的发生
    ```bash
    gdb -q ./main \
    -ex "catch throw" \
    -ex "run" \
    -ex "bt"
    Reading symbols from ./main...
    Catchpoint 1 (throw)
    Starting program: /home/yuxiangw/GitHub/learning_book/docs/booknotes/debug_hacks/gdb/code/catch_throw/main 
    start

    Catchpoint 1 (exception thrown), 0x00007ffff7e448b2 in __cxa_throw ()
    from /lib/x86_64-linux-gnu/libstdc++.so.6
    #0  0x00007ffff7e448b2 in __cxa_throw () from /lib/x86_64-linux-gnu/libstdc++.so.6
    #1  0x00005555555552e8 in throwException () at main.cpp:8
    #2  0x0000555555555314 in main () at main.cpp:17
    ```

* `catch syscall <name>`，捕获系统调用，系统调用名字可查看"/usr/include/asm/unistd.h"

* `catch fork`，捕获`fork`调用
    * [实例代码](./code/catch_fork/main.cpp)被GDB捕获了`fork`的调用
    ```bash
    gdb -q ./main \
    -ex "catch fork" \
    -ex "run" \
    -ex "bt"
    Reading symbols from ./main...
    Catchpoint 1 (fork)
    Starting program: /home/yuxiangw/GitHub/learning_book/docs/booknotes/debug_hacks/gdb/code/catch_fork/main 
    Fork example

    Catchpoint 1 (forked process 1767571), arch_fork (ctid=0x7ffff7fb2810) at ../sysdeps/unix/sysv/linux/arch-fork.h:49
    49      ../sysdeps/unix/sysv/linux/arch-fork.h: No such file or directory.
    #0  arch_fork (ctid=0x7ffff7fb2810) at ../sysdeps/unix/sysv/linux/arch-fork.h:49
    #1  __libc_fork () at ../sysdeps/nptl/fork.c:76
    #2  0x0000555555555239 in main () at main.cpp:10
    ```

* `catch signal`，捕获信号的发生
    * [实例代码](./code/catch_signal/main.cpp)被GDB捕获了"SIGUSR1"和"SIGCHLD"的信号
    ```bash
    gdb -q ./main \
    -ex "catch signal" \
    -ex "run" \
    -ex "bt" \
    -ex "c" \
    -ex "bt"
    Reading symbols from ./main...
    Catchpoint 1 (standard signals)
    Starting program: /home/yuxiangw/GitHub/learning_book/docs/booknotes/debug_hacks/gdb/code/catch_signal/main 
    Catch signal example
    [Detaching after fork from child process 1796689]
    This is parent process to handle SIGUSR1 signal [process PID: 1796685, signal sender PID: 1796689].
    This is child process to send SIGUSR1 signal [process PID: 1796689, parent process PID: 1796685].

    Send SIGUSR1 signal successfully.
    Child process is Done!
    Catchpoint 1 (signal SIGUSR1), 0x00007ffff7ea4c2a in __GI___wait4 (pid=1796689, stat_loc=0x7fffffffdbfc, 
        options=0, usage=0x0) at ../sysdeps/unix/sysv/linux/wait4.c:27
    27      ../sysdeps/unix/sysv/linux/wait4.c: No such file or directory.
    #0  0x00007ffff7ea4c2a in __GI___wait4 (pid=1796689, stat_loc=0x7fffffffdbfc, options=0, usage=0x0)
        at ../sysdeps/unix/sysv/linux/wait4.c:27
    #1  0x000055555555533d in main () at main.cpp:27
    Continuing.

    Catchpoint 1 (signal SIGCHLD), SIGUSR1_handler (signo=0) at main.cpp:9
    9       {
    #0  SIGUSR1_handler (signo=0) at main.cpp:9
    #1  <signal handler called>
    #2  0x00007ffff7ea4c28 in __GI___wait4 (pid=1796689, stat_loc=0x7fffffffdbfc, options=0, usage=0x7ffff7ea4c2a <__GI___wait4+26>) at ../sysdeps/unix/sysv/linux/wait4.c:27
    #3  0x000055555555533d in main () at main.cpp:27
    ```

## 观察状态

### 显示栈帧(backtrace)
* `bt N`，显示开头N个栈帧
* `bt full`，不仅显示backtrace，还显示局部变量
* `frame <栈帧号>`，选择要显示的栈帧，可观察当前栈帧上的变量

### 显示寄存器
* `info reg`，显示各寄存器的值
    * 可缩写为`i r`，例如查看PC寄存器的值，可输入：`i r pc`
    * 查看所有寄存器的值：`info all-registers`
* `p $eax`，显示eax寄存器的值

### 显示变量
* `p/<格式> <变量>`，按指定格式显示变量的值
    * GDB支持的常见格式，其他格式可参考[手册](https://sourceware.org/gdb/onlinedocs/gdb/Output-Formats.html#Output-Formats)

    |格式 |说明                             |
    |-    |-                               |
    |x    | 显示为十六进制数                 |
    |a    | 地址                            |
    |c    | 显示为字符                      |
    |s    | 显示为字符串                    |
    |i    | 显示为机器语言(只在`x`命令中有效) |

* `p *<数组名>@<数组长度>`，打印数组内容
* `x/nfu <地址>`，显示内存内容
    * n表示显示的次数，默认是1
    * f是显示格式，默认是x
    * u表示单位，其默认值和显示格式有关，可取值：b,h,w,g
* `explore <变量/表达式/类型>`，交互式地提供参数的信息

## 修改状态

* `set variable <变量>=<表达式>`，改变变量的值

## 多线程调试

* `info inferiors`，查看正在被GDB调试的进程
* `info threads`，查看所有线程的状态信息
* `thread <threadno>`，切换到编号为"threadno"的线程
* `thread apply <threadno> <command>`，只让编号为"threadno"的线程执行GDB命令
* `set scheduler-locking on`，只运行在当前线程中，不切换到他线程执行任何GDB命令
    * [实例代码](./code/threads/main.cpp)创建一个主线程和四个子线程
    ```bash
    gdb -q ./main \
    -ex "b signals" \
    -ex "run" \
    -ex "info threads" \
    -ex "info inferiors"
    Reading symbols from ./main...
    Breakpoint 1 at 0x1468: file main.cpp, line 18.
    Starting program: /home/yuxiangw/GitHub/learning_book/docs/booknotes/debug_hacks/gdb/code/threads/main 
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
    [New Thread 0x7ffff7a12700 (LWP 1821943)]
    Waiting... 
    [New Thread 0x7ffff7211700 (LWP 1821944)]
    Waiting... 
    [New Thread 0x7ffff6a10700 (LWP 1821945)]
    Waiting... 
    [New Thread 0x7ffff620f700 (LWP 1821946)]
    [Switching to Thread 0x7ffff620f700 (LWP 1821946)]

    Thread 5 "main" hit Breakpoint 1, signals () at main.cpp:18
    18      {
    Id   Target Id                                  Frame 
    1    Thread 0x7ffff7a13740 (LWP 1821939) "main" __pthread_clockjoin_ex (threadid=140737347921664, 
        thread_return=0x0, clockid=<optimized out>, abstime=<optimized out>, block=<optimized out>)
        at pthread_join_common.c:145
    2    Thread 0x7ffff7a12700 (LWP 1821943) "main" futex_wait_cancelable (private=<optimized out>, expected=0, 
        futex_word=0x55555555a188 <cv+40>) at ../sysdeps/nptl/futex-internal.h:183
    3    Thread 0x7ffff7211700 (LWP 1821944) "main" futex_wait_cancelable (private=<optimized out>, expected=0, 
        futex_word=0x55555555a188 <cv+40>) at ../sysdeps/nptl/futex-internal.h:183
    4    Thread 0x7ffff6a10700 (LWP 1821945) "main" futex_wait_cancelable (private=<optimized out>, expected=0, 
        futex_word=0x55555555a188 <cv+40>) at ../sysdeps/nptl/futex-internal.h:183
    * 5    Thread 0x7ffff620f700 (LWP 1821946) "main" signals () at main.cpp:18
    Num  Description       Executable        
    * 1    process 1821939   /home/yuxiangw/GitHub/learning_book/docs/booknotes/debug_hacks/gdb/code/threads/main 
    ```

## 其他常见命令

* `info sharedlibrary`，打印已经装载的动态库
* `info proc`，打印进程信息
* `info proc mappings`，查看进程的内存映射，即显示进程对应的/proc/<PID>/maps的信息
* `disas /s <函数/地址>`，查看汇编代码，其中`/s`表示同时显示source+assembly代码
* `set print elements unlimited`，不限制数组中元素显示的个数，默认情况下，GDB打印数组时，只一次性显示部分元素

## 参考
* ["GDB的用户手册"](https://sourceware.org/gdb/current/onlinedocs/gdb/Command-and-Variable-Index.html#Command-and-Variable-Index)