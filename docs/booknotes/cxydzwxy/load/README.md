# 装载

> [《程序员的自我修养--链接、装载与库》 - 俞甲子，石凡，潘爱民](https://1drv.ms/b/s!AkcJSyT7tq80cUuHb2eRcJkkBjM?e=YUwBqB)，第六章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/load/code)中找到

## 进程虚拟地址空间

### 32位
32位系统的地址空间是4GB(2^32)，参见[文档](https://gabrieletolomei.wordpress.com/miscellanea/operating-systems/in-memory-layout/)。其中，1GB是内核空间，3GB是用户空间：

* 内核空间：0xC0000000 ~ 0xFFFFFFFF
* 用户空间：0x00000000 ~ 0xBFFFFFFF
    * `.text`段的加载地址是：0x08048000

![vm_32bit](./images/vm_32bit.png)


### 64位
64位系统的最大地址空间是16EB(2^64)，参见[文档](https://en.wikipedia.org/wiki/X86-64#Virtual_address_space_details)。AMD架构规定了3种64位系统的虚拟地址空间规范，分别是：48-bit实现，57-bit实现和64-bit实现。

![vm_64bit](./images/vm_64bit.png)

以48-bit实现为例，可用地址空间为两个128TB(2^47)范围(如下图)，分别是：

* 内核空间：0xFFFF_8000_0000_0000 ~ 0xFFFF_FFFF_FFFF_FFFF
* 用户空间：0x0000_0000_0000_0000 ~ 0x0000_7FFF_FFFF_FFFF
    * 和32位系统一样，用户空间也包含了栈区、堆区、代码段、数据段等

![vm_48bit](./images/vm_48bit.png)

[例子"vm_addr"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/load/code/vm_addr)通过打印`__executable_start`变量的值，以查看进程被加载的起始地址。默认情况下，gcc会利用ASLR，随机进程的加载地址。因此，需要手动指定链接脚本(`-T`)，以固定进程加载地址，便于观察。例子中的链接脚本[main.lds](./code/vm_addr/main.lds)和默认链接脚本`ld -verbose`一致，指定了加载地址`.`：
```cpp
...
SECTIONS
{
  PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x400000)); . = SEGMENT_START("text-segment", 0x400000) + SIZEOF_HEADERS;
  ...
}
```
程序运行结果如下，其中：

* 加载地址由`SEGMENT_START("text-segment", 0x400000) + SIZEOF_HEADERS`指定，运行结果是`0x400318`
* `__executable_start`变量的值由`SEGMENT_START("text-segment", 0x400000)`指定，运行结果是`0x400000`
* 进程运行时加载页的起始地址是`0x400000`，由链接时指定的加载地址`0x400318`决定

```cpp
> ./main
Executable Start 0x400000
Text End 0x401285 0x401285 0x401285
Data End 0x404030 0x404030
Executable End 0x404038 0x404038

> objdump -h ./main
./main:     file format elf64-x86-64
Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .interp       0000001c  0000000000400318  0000000000400318  00000318  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 .note.gnu.property 00000020  0000000000400338  0000000000400338  00000338  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA

> make debug
Mapped address spaces:
          Start Addr           End Addr       Size     Offset objfile
            0x400000           0x401000     0x1000        0x0 ./main
            0x401000           0x402000     0x1000     0x1000 ./main
            0x402000           0x403000     0x1000     0x2000 ./main
            0x403000           0x404000     0x1000     0x2000 ./main
            0x404000           0x405000     0x1000     0x3000 ./main
      0x7ffff7dbf000     0x7ffff7de4000    0x25000        0x0 /usr/lib/x86_64-linux-gnu/libc-2.31.so
```


## 装载的方式

## 可执行文件的装载

## 进程虚拟空间分布

## 内核装载ELF过程

