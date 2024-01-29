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



