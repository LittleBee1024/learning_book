# 调试符号和调试器

> [《高效C/C++调试》 - 严琦](http://www.tup.tsinghua.edu.cn/Wap/tsxqy.aspx?id=10204101)，第一章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/01/code)中找到

## 调试符号

常见的调试符号有以下几类：

* 全局函数和变量
* 源文件和行信息
* 类型信息
* 静态函数和局部变量
* 架构和编译器依赖信息

### DWARF格式

[DWARF格式](https://dwarfstd.org/)是Linux系统中的标准调试符号格式。DWARF以树形结构组织调试符号。每一个树节点都是一个调试信息记录(Debug Information Entry, DIE)。

下面我们以[foo.cpp](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/01/code/dwarf/foo.cpp)为例介绍DWARF格式：

```cpp linenums="1"
int gInt = 1;
int GlobalFunc(int i)
{
    return i + gInt;
}
```

我们可以用`readelf --debug-dump foo.o`命令查看调试符号的各种信息。调试符号被划分为多个节(section)：

* `.debug_abbrev`节
    * 缩略表中记录DIE的模板，用于真实DIE项的索引
* `.debug_info`节
    * 包含了调试符号的核心内容，其中每个DIE都指定了其在缩略表中的索引
* `.debug_line`节
    * 用于将指令地址映射到源代码行号
* `.eh_frame`节(或`.debug_frame`节)
    * 描述了函数的栈帧和寄存器的分配方式

以`GlobalFunc`函数的参数`int i`为例，其在汇编代码"foo.s"中编码如下：

```asm
.uleb128 0x5
.string	"i"
.byte	0x1
.byte	0x2
.byte	0x14
.long	0x44
.uleb128 0x2
.byte	0x91
.sleb128 -20
.byte	0
```

参数`int i`的DIE在`.debug_info`段中内容如下：
```
 <2><6d>: Abbrev Number: 5 (DW_TAG_formal_parameter)
    <6e>   DW_AT_name        : i
    <70>   DW_AT_decl_file   : 1
    <71>   DW_AT_decl_line   : 2
    <72>   DW_AT_decl_column : 20
    <73>   DW_AT_type        : <0x44>
    <77>   DW_AT_location    : 2 byte block: 91 6c 	(DW_OP_fbreg: -20)
```

其对应的原始数据可通过`objdump -s --section=.debug_info foo.o`命令查看：

* `05 6900 01 02 14 44000000 02 916c 00`

参数`int i`的DIE含义包括：

* 这个DIE在缩略表中的索引是5
* 参数名称是“i”
* 它出现在第1个文件中
* 它位于该文件的第2行，第20列
* 参数的类型有另一个DIE(引用为0x44)来描述
* 参数的存储位置由接下来的2字节指示，这对应于相对于寄存器fbreg的偏移量-20
* 最后这个DIE以一共0字节结束

## 调试器内部

Linux内核提供了一个系统调用`ptrace`，允许一个进程(调试器或其他工具，如系统调用追踪器strace)查询和控制另一个进程(被调试程序)的执行。`ptrace`提供了以下功能：

* 读写被调试进程内存地址空间中的内容，包括文本和数据段
* 查询和修改被调试进程的用户区域信息，例如寄存器等
* 查询和修改被调试进程的信号信息和设置

GDB的断点就是利用`ptrace`实现的。假设函数`main`的起始地址为`0x400590`，GDB在`main`处设置断点，接下来的过程大致如下：

* GDB读取地址`0x400590`处的代码
* GDB使用`PTRACE_POKEDATA`请求替换该地址的代码，将最低字节更改为`0xcc`。`0xcc`是一种特殊的陷阱指令[INT3](https://stackoverflow.com/questions/61816297/what-is-int-3-really-supposed-to-do)，当Linux系统遇到`INT3`指令，会产生`SIGTRAP`信号。调试器接收到`SIGTRAP`信号后，会判断是否存在断点或监控点，从而做出对应的动作
* 被调试的程序执行陷阱指令`0xcc`，进入暂停状态，并向GDB发送`SIGTRAP`信号
* GDB收到信号后，采取相应的动作。例如，如果存在断点且满足条件，GDB会等待用户调试命令
* 如果我们继续执行程序，GDB会替换回`0x400590`处的原始指令，并使用`PTRACE_SINGLESTEP`请求执行单个指令

## 调试技巧

### add-symbol-file

如果我们要调试的程序没有`debug`信息，可以通过`add-symbol-file`命令手动导入`debug`信息进行调试。["add-symbol-file"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/01/code/add-symbol-file)例子展示了如何利用`add-symbol-file`命令调试缺失调试信息的程序。

需要**注意**的是：要想成功导入调试信息，需保证调试信息必须和被调试程序一致，包括优化等级等。否则，断点仍会因为地址不对而无法工作。

```bash
sh> gdb -q ./main
Reading symbols from ./main...
(No debugging symbols found in ./main)
(gdb) b main
Breakpoint 1 at 0x401136
# 由于可执行文件“main”不包含调试信息，所以断点信息不全
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000401136 <main+4>
# 用“add-symbol-file”导入包含调试信息的“main_d”
(gdb) add-symbol-file main_d
Reading symbols from main_d...
# 断点在"main.c:6"处
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x000000000040113a in main() at main.c:6
# 启动程序后，停在了断点处
(gdb) r
Breakpoint 1, main () at main.c:6
6          init();
# 由于“libsym.so”不包含调试信息，所以无法在“mm_symbol.c:init”处打断点
(gdb) b mm_symbol.c:init
No source file named mm_symbol.c.
Make breakpoint pending on future shared library load? (y or [n]) y
Breakpoint 2 (mm_symbol.c:init) pending.
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x000000000040113a in main() at main.c:6
        breakpoint already hit 1 time
2       breakpoint     keep y   <PENDING>          mm_symbol.c:init
(gdb) info sharedlibrary 
From                To                  Syms Read   Shared Object Library
0x00007ffff7dd5030  0x00007ffff7df4214  Yes (*)     /lib64/ld-linux-x86-64.so.2
0x00007ffff7ff1040  0x00007ffff7ff115d  Yes (*)     ./libsym.so
# 导入"libsym_d.so"的调试信息“libsym.sym”到指定地址“0x00007ffff7ff1040”
# 共享库地址可通过“info sharedlibrary”找到，“libsym.sym”由
# “objcopy --only-keep-debug libsym_d.so libsym.sym”
# 命令产生，当然也可以直接从“libsym_d.so”导入调试信息。
(gdb) add-symbol-file libsym.sym 0x00007ffff7ff1040
Reading symbols from libsym.sym...
# 调试信息导入成功后，成功在“mm_symbol.c:init”处打上了断点
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x000000000040113a in main() at main.c:6
        breakpoint already hit 1 time
2       breakpoint     keep y   0x00007ffff7ff10f9 in init() at mm_symbol.c:7
(gdb) c
Continuing.
Breakpoint 2, init () at mm_symbol.c:7
7          g_blocks[0].size = 1;
(gdb) c
Continuing.
g_blocks[0]=0x404040, g_blocks[0].size=1, g_blocks[0].prev=(nil), g_blocks[0].next=0x404058
g_blocks[1]=0x404058, g_blocks[1].size=2, g_blocks[1].prev=0x404040, g_blocks[1].next=(nil)
[Inferior 1 (process 22107) exited normally]
```

### 条件断点

下面是常见的条件断点设置方法：

```bash
# 忽略断点(1) 100次
(gdb) ignore 1 100
# 在变量index为5的条件下在函数foo入口处停止
(gdb) break foo if index==5
# 在指令地址0x12345678处设置断点，且函数`GetRefCount`返回值为0时才生效
# GDB还提供了常用的函数用于设置条件断点，如字符串比较函数“$_streq”，更多函数可参见：
#   https://sourceware.org/gdb/current/onlinedocs/gdb.html/Convenience-Funs.html
(gdb) break *0x12345678 if GetRefCount(this)==0
```

### 监测点

断点可以设置在代码中，也可以设置在数据对象上。后者被称为监测点，或者数据断点。

断点和监测点使用不同的机制实现。如前面所述，调试器是通过将指定位置的指令替换为短陷阱指令来设置断点，原来的指令代码被保存在缓冲区。当程序执行到陷阱指令，也就是达到断点时，内核会停止程序运行并通知调试器，后者从等待中醒来，显示所跟踪程序的状态，然后等待用户的下一条命令。

监测点不能用指令断点的方法实现，因为数据对象是不可执行的。所以它的实现是要么定期地(软件模式)查询数据的值，要么使用CPU支持的调试寄存器(硬件模式)。软件监测点是通过单步运行程序并在每一步检查被跟踪的变量来实现的，这种方式使程序比正常运行要慢数百倍。

由于单步运行(软件模式)不能保证在多线程环境下的结果一致性，因此在多线程和多处理器的环境下，这种方法可能无法捕获到数据被访问的瞬间。硬件监测点则没有这个问题，因为被跟踪的变量的计算是由硬件完成的，调试器不用介入，它根本不会减慢程序的执行速度。但是由于CPU调试寄存器个数的限制，如果监测点表达式很复杂或需要设置很多监测点，调试器会**隐式**地回归到软件监测点。

["watch"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/01/code/watch)例子展示了如何利用`watch sum if times >= 2`命令设置监控点，监控`sum`值的变化，并在`times`大于等于2的条件下触发。

```bash
sh> gdb -q ./main -ex 'b main' -ex 'run' -ex 'watch sum if times >= 2' -ex 'c'
Breakpoint 1, main () at main.cpp:5
5           int sum = 0;
Hardware watchpoint 2: sum
Continuing.
1: sum=1
Hardware watchpoint 2: sum
Old value = 1
New value = 3
main () at main.cpp:14
14          printf("%d: sum=%d\n", times, sum);
# 在"times"等于2，"sum"等于3处，成功停住
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x000000000040112a in main() at main.cpp:5
        breakpoint already hit 1 time
2       hw watchpoint  keep y                      sum
        stop only if times >= 2
        breakpoint already hit 1 time
```
