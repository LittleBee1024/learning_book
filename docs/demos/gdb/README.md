# GDB

> GDB的高级用法，所有样例可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/demos/gdb/code)中找到

## 简介
GDB用于程序调试，常见的操作有：

* 启动调试程序，并可附加启动参数
* 根据指定条件，暂停程序的运行
* 查看暂停处程序运行的各种状态
* 按需要修改程序的状态后，继续运行程序以观察状态的变化

> 可通过["GDB的用户手册"](https://sourceware.org/gdb/current/onlinedocs/gdb/Command-and-Variable-Index.html#Command-and-Variable-Index)查看GDB所有命令的用法

## 启动与执行

* GDB启动进程
    * `gdb <EXE>`
    * `gdb --args <EXE> <args...>`
* GDB挂载到运行的进程
    * `gdb -p <PID>`
* 执行
    * `continue <次数>`，达到指定次数前，执行到断点时不暂停
    * `finish`，执行完当前函数后暂停
    * `until`，执行完当前代码块后暂停，如果是循环，执行完循环后暂停

## 设置断点

* 设置普通断点(break point)
    * `break <函数名>`
    * `break <文件名:行号>`
    * `break <*地址>`
    * `break <+/-偏移量>`
* 设置条件断点(conditional break point)
    * `break <断点> if <条件>`
* 设置监视点(watch point)
    * `watch <变量/地址>`，监视变量或地址，当发生变化时暂停程序
* 设置捕获点(catch point)
    * `catch throw`，捕获C++程序的异常，当发生异常时，暂停程序
    * `catch syscall <name>`，捕获系统调用，系统调用名字可查看"/usr/include/asm/unistd.h"
    * `catch signal`，捕获信号的发生

## 观察状态

* 显示栈帧(backtrace)
    * `bt N`，显示开头N个栈帧
    * `bt full`，不仅显示backtrace，还显示局部变量
    * `frame <栈帧号>`，选择要显示的栈帧，可观察当前栈帧上的变量
* 显示寄存器
    * `info reg`，显示各寄存器的值
    * `p $eax`，显示eax寄存器的值
* 显示变量
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

* 改变变量的值
    * `set variable <变量>=<表达式>`

## 其他常见命令

* `info sharedlibrary`，打印已经装载的动态库
* `info proc`，打印进程信息
* `info proc mappings`，查看进程的内存映射，即显示进程对应的/proc/<PID>/maps的信息
* `disas /s <函数/地址>`，查看汇编代码，其中`/s`用于同时显示source+assembly代码