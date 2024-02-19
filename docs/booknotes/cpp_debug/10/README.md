# 内存调试工具

> [《高效C/C++调试》 - 严琦](http://www.tup.tsinghua.edu.cn/Wap/tsxqy.aspx?id=10204101)，第十章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/10/code)中找到

内存调试工具大致分三类：

* 填充字节方法
    * 在每个内存块的开头或末尾添加额外的**填充字节**，如果这些填充字节发生改变，说明存在非法的内存修改
* 系统保护页方法
    * 在可能被越界的内存块前后设置一个不可访问的系统保护页，如果程序试图访问受保护的内存，系统就可以通过硬件检测到这种操作
* 动态二进制分析
    * 常见的内存检测工具`Valgrind`和`Google Address Sanitizer`都是通过此方法实现的
    * 采用**影子内存**来跟踪程序的实时内存使用

## ptmalloc's MALLOC_CHECK_

`glibc`内存管理器`ptmalloc`实现了一个基于填充的简单且非常有用的调试特性。我们可以通过将环境变量`MALLOC_CHECK_`设置为非零数值来启用它。该值的最低两位决定了`ptmalloc`在检测到错误时应采取的操作：

* 如果最低位设置了(例如1)，那么每次检测到错误时都会在stderr上打印错误消息
* 如果第二位设置了(例如2)，那么只要核心转储打开，`ptmalloc`就会在检测到错误时终止进程并生成一个核心转储文件


[例子“mem_check”](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/10/code/mem_check)展示了`MALLOC_CHECK_`环境变量的作用。

```cpp linenums="1"
#include <stdio.h>

int main()
{
   int *array = new int[100];
   array[100] = 0; // error
   delete[] array;
   return 0;
}
```

上面的代码存在越界访问的问题，正常执行程序并不会报任何错误，但通过设置`MALLOC_CHECK_`环境变量，可检测出程序存在错误：

```bash
sh> ./main
sh> env MALLOC_CHECK_=1 ./main
*** Error in `./main': free(): invalid pointer: 0x000000000149cc20 ***
# 由于没有开启核心存储，所以此处只是因内存非法访问而中断了程序
sh> env MALLOC_CHECK_=2 ./main
Abort
```

## Google Address Sanitizer

Google Address Sanitizer(通常简称为`ASan`)是一个快速的内存错误检测器，它能发现如下问题：

* 内存泄漏
* 缓冲区溢出
* 释放后使用
* 使用未初始化的内存
* 空指针引用
* 栈缓冲溢出

为了使用`ASan`，需要在编译时加上`-fsanitize=address`标志，同时加上`-g`生成调试信息以方便`ASan`打印错误报告。

上面的[例子“mem_check”](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/10/code/mem_check)同样可以用`ASan`找到数组越界的问题(发生在`main.cpp`的第六行)：

```bash
g++ -fsanitize=address -g main.cpp -o main
./main
=================================================================
==101388==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x6140000001d0 at pc 0x0000004011ca bp 0x7ffd8bc52d20 sp 0x7ffd8bc52d18
WRITE of size 4 at 0x6140000001d0 thread T0
    #0 0x4011c9 in main /lan/cva/hsv-apfw/yuxiangw/GitHub/starter-demo/effective_debugging/malloc_check/main.cpp:6
    #1 0x2b98af7ad3d4 in __libc_start_main (/lib64/libc.so.6+0x223d4)
    #2 0x4010c8  (/lan/cva/hsv-apfw/yuxiangw/GitHub/starter-demo/effective_debugging/malloc_check/main+0x4010c8)

0x6140000001d0 is located 0 bytes to the right of 400-byte region [0x614000000040,0x6140000001d0)
allocated by thread T0 here:
    #0 0x2b98aeb1b37f in operator new[](unsigned long) /tmp/gcc-v9.3.0p7/gcc.source/libsanitizer/asan/asan_new_delete.cc:107
    #1 0x401183 in main /lan/cva/hsv-apfw/yuxiangw/GitHub/starter-demo/effective_debugging/malloc_check/main.cpp:5
    #2 0x2b98af7ad3d4 in __libc_start_main (/lib64/libc.so.6+0x223d4)

SUMMARY: AddressSanitizer: heap-buffer-overflow /lan/cva/hsv-apfw/yuxiangw/GitHub/starter-demo/effective_debugging/malloc_check/main.cpp:6 in main
Shadow bytes around the buggy address:
  0x0c287fff7fe0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c287fff7ff0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c287fff8000: fa fa fa fa fa fa fa fa 00 00 00 00 00 00 00 00
  0x0c287fff8010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c287fff8020: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0c287fff8030: 00 00 00 00 00 00 00 00 00 00[fa]fa fa fa fa fa
  0x0c287fff8040: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c287fff8050: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c287fff8060: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c287fff8070: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c287fff8080: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==101388==ABORTING
```
