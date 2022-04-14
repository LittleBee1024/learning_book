# 动态链接

> [《程序员的自我修养--链接、装载与库》 - 俞甲子，石凡，潘爱民](https://1drv.ms/b/s!AkcJSyT7tq80cUuHb2eRcJkkBjM?e=YUwBqB)，第七章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code)中找到

## 基本实现
在Linux系统中，ELF动态链接文件被称为动态共享对象(DSO, Dynamic Shared Objects)，简称**共享对象**，它们一般都是以".so"为扩展名的一些文件。

程序和动态链接库之间的链接工作是由动态链接器完成的。不同于静态链接器，动态链接器把链接过程从程序装载前推迟到了装载的时候。这种方法称为延迟绑定(Lazy Binding)，虽然损失了一些运行时的性能，但是提高了灵活性。

### 简单的例子
[示例](./code/simple)中的两个可执行文件`program1`和`program2`都动态链接到了动态共享对象`libfoo.so`。当程序模块`program1.c`被编译成`program1.o`时，编译器不知道`foobar()`函数的地址。当链接器将`program1.o`链接成可执行文件时，通过函数符号信息，链接器可判断`foobar()`的性质，

* 如果`foobar()`是一个定义在其他静态目标模块中的函数，那么链接器将会按照静态链接的规则，将`program1.o`中的foobar地址引用重定位
* 如果`foobar()`是一个定义在某个动态共享对象中的函数，那么链接器就会将这个符号的引用标记位一个动态链接的符号，不对它进行地址重定位，把这个过程留到装载时再进行

通过GDB命令`i proc mapping`，可得到进程`program1`的虚拟地址映射，其分别链接了`libc-2.31.so`，`libfoo.so`和`ld-2.31.so`三个动态链接库。
```asm
        Start Addr           End Addr       Size     Offset objfile
    0x555555554000     0x555555555000     0x1000        0x0 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/program1
    0x555555555000     0x555555556000     0x1000     0x1000 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/program1
    0x555555556000     0x555555557000     0x1000     0x2000 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/program1
    0x555555557000     0x555555558000     0x1000     0x2000 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/program1
    0x555555558000     0x555555559000     0x1000     0x3000 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/program1
    0x7ffff7db7000     0x7ffff7dba000     0x3000        0x0 
    0x7ffff7dba000     0x7ffff7ddf000    0x25000        0x0 /usr/lib/x86_64-linux-gnu/libc-2.31.so
    0x7ffff7ddf000     0x7ffff7f57000   0x178000    0x25000 /usr/lib/x86_64-linux-gnu/libc-2.31.so
    0x7ffff7f57000     0x7ffff7fa1000    0x4a000   0x19d000 /usr/lib/x86_64-linux-gnu/libc-2.31.so
    0x7ffff7fa1000     0x7ffff7fa2000     0x1000   0x1e7000 /usr/lib/x86_64-linux-gnu/libc-2.31.so
    0x7ffff7fa2000     0x7ffff7fa5000     0x3000   0x1e7000 /usr/lib/x86_64-linux-gnu/libc-2.31.so
    0x7ffff7fa5000     0x7ffff7fa8000     0x3000   0x1ea000 /usr/lib/x86_64-linux-gnu/libc-2.31.so
    0x7ffff7fa8000     0x7ffff7fac000     0x4000        0x0 
    0x7ffff7fc2000     0x7ffff7fc3000     0x1000        0x0 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/libfoo.so
    0x7ffff7fc3000     0x7ffff7fc4000     0x1000     0x1000 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/libfoo.so
    0x7ffff7fc4000     0x7ffff7fc5000     0x1000     0x2000 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/libfoo.so
    0x7ffff7fc5000     0x7ffff7fc6000     0x1000     0x2000 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/libfoo.so
    0x7ffff7fc6000     0x7ffff7fc7000     0x1000     0x3000 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code/simple/libfoo.so
    0x7ffff7fc7000     0x7ffff7fc9000     0x2000        0x0 
    0x7ffff7fc9000     0x7ffff7fcd000     0x4000        0x0 [vvar]
    0x7ffff7fcd000     0x7ffff7fcf000     0x2000        0x0 [vdso]
    0x7ffff7fcf000     0x7ffff7fd0000     0x1000        0x0 /usr/lib/x86_64-linux-gnu/ld-2.31.so
    0x7ffff7fd0000     0x7ffff7ff3000    0x23000     0x1000 /usr/lib/x86_64-linux-gnu/ld-2.31.so
    0x7ffff7ff3000     0x7ffff7ffb000     0x8000    0x24000 /usr/lib/x86_64-linux-gnu/ld-2.31.so
    0x7ffff7ffc000     0x7ffff7ffd000     0x1000    0x2c000 /usr/lib/x86_64-linux-gnu/ld-2.31.so
    0x7ffff7ffd000     0x7ffff7ffe000     0x1000    0x2d000 /usr/lib/x86_64-linux-gnu/ld-2.31.so
    0x7ffff7ffe000     0x7ffff7fff000     0x1000        0x0 
    0x7ffffffde000     0x7ffffffff000    0x21000        0x0 [stack]
0xffffffffff600000 0xffffffffff601000     0x1000        0x0 [vsyscall]
```


