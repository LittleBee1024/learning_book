# 动态链接

> [《程序员的自我修养--链接、装载与库》 - 俞甲子，石凡，潘爱民](https://1drv.ms/b/s!AkcJSyT7tq80cUuHb2eRcJkkBjM?e=YUwBqB)，第七、八章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code)中找到

## 基本实现
在Linux系统中，ELF动态链接文件被称为动态共享对象(DSO, Dynamic Shared Objects)，简称**共享对象**，它们一般都是以".so"为扩展名的一些文件。

程序和动态链接库之间的链接工作是由动态链接器完成的。不同于静态链接器，动态链接器把链接过程从程序装载前推迟到了装载的时候。这种方法称为延迟绑定(Lazy Binding)，虽然损失了一些运行时的性能，但是提高了灵活性。

### 简单的例子
[例子"simple"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/simple)中的两个可执行文件`program1`和`program2`都动态链接到了动态共享对象`libfoo.so`。当程序模块`program1.c`被编译成`program1.o`时，编译器不知道`foobar()`函数的地址。当链接器将`program1.o`链接成可执行文件时，通过函数符号信息，链接器可判断`foobar()`的性质，

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

## 重定位

### 装载时重定位
如果动态模块中有绝对地址引用，由于我们无法在链接时知道动态库的装载地址，因此需采用**装载时重定位(Load Time Relocation)**技术，在动态库装载的时候，修改其绝对地址引用，以保证地址的正确性。

例如，一个动态库在编译时假设装载目标地址是0x1000，但实际装载地址是0x4000，那么动态库中所有绝对地址引用都需要加上0x3000的偏移量。

### 地址无关代码
装载时重定位技术的最大缺点是，需要根据装载地址的不同，在装载时修改指令。

**地址无关代码(PIC, Position-independent Code)**解决了此问题。其基本思想是把指令中那些需要被修改的部分分离出来，跟数据部分放在一起。多个进程之间共享不变的指令部分，而各自拥有可变数据部分。GCC编译时，`-fPIC`编译选项指定了编译出的代码为地址无关代码，在编译动态共享库时，需要加上此选项。

我们把共享对象模块中的地址引用按照是否跨模块分成：模块内部引用和模块外部引用，按照不同的引用方式又可分为指令引用和数据访问，存在如下所示的四种情况：
```cpp
static int a;
extern int b;
extern void ext();

void bar()
{
    a = 1; // 模块内部数据访问
    b = 2; // 模块间数据访问
}

void foo()
{
    bar(); // 模块内部调用或跳转
    ext(); // 模块间调用或跳转
}
```

由于编译器的优化作用，我们需要通过间接依赖的[例子"secondary_dep"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/secondary_dep)才能达到模块间数据访问的效果。如果是直接依赖的[例子"direct_dep"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/direct_dep)，都被优化成了模块内的数据访问。

* [实例"secondary_dep"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/secondary_dep) - 模块内数据访问`a`和模块间数据访问`b`
    * 对于模块内数据访问，直接用相对地址即可得到存储位置。例如，变量`a`的地址`0x7ffff7fc603c`在`bar`函数中直接被引用
    * 对于模块间数据访问，需要全局偏移表(Global Offset Table, GOT)的帮助。例如，`b`真正的地址是`0x7ffff7db902c`，该地址存于GOT地址`0x7ffff7fc5fe0`中，`bar`函数引用的时GOT地址`0x7ffff7fc5fe0`
    ```asm
    (gdb) disas bar
    Dump of assembler code for function bar:
        0x00007ffff7fc3159 <+0>:     endbr64 
        0x00007ffff7fc315d <+4>:     push   %rbp
        0x00007ffff7fc315e <+5>:     mov    %rsp,%rbp
        0x00007ffff7fc3161 <+8>:     movl   $0x1,0x2ed1(%rip)        # 0x7ffff7fc603c <a>
        0x00007ffff7fc316b <+18>:    mov    0x2e6e(%rip),%rax        # 0x7ffff7fc5fe0
        0x00007ffff7fc3172 <+25>:    movl   $0x2,(%rax)

    # 打印变量a的地址0x7ffff7fc603c内容
    (gdb) p/x *0x7ffff7fc603c
    $1 = 0x1
    (gdb) info sym &a
    a in section .bss of ./libouter.so
    # 变量a无须经过GOT映射
    (gdb) p/x &a
    $2 = 0x7ffff7fc603c

    # 打印变量b的GOT地址0x7ffff7fc5fe0内容
    (gdb) p/x *(uint64_t *)0x7ffff7fc5fe0
    $3 = 0x7ffff7db902c
    (gdb) info sym &b
    b in section .bss of ./libinner.so
    # 变量b的地址和GOT地址0x7ffff7fc5fe0的内容相同
    (gdb) p/x &b
    $4 = 0x7ffff7db902c
    ```
* [实例"direct_dep"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/direct_dep) - 模块内调用`bar`和模块间调用`ext`
    * 模块内函数`bar`直接使用真实的地址
    * 模块间函数`ext`的调用利用了[延迟绑定PLT](#plt)技术，函数调用需经过：`ext@plt` -> `ext@got.plt` -> 真正的地址`0x7ffff7fc3119`
    ```asm
    (gdb) disas foo
    Dump of assembler code for function foo:
        0x00005555555551a0 <+0>:     endbr64 
        0x00005555555551a4 <+4>:     push   %rbp
        0x00005555555551a5 <+5>:     mov    %rsp,%rbp
        0x00005555555551a8 <+8>:     mov    $0x0,%eax
        0x00005555555551ad <+13>:    callq  0x555555555070 <ext@plt>
        0x00005555555551b2 <+18>:    mov    $0x0,%eax
        0x00005555555551b7 <+23>:    callq  0x555555555169 <bar>
        0x00005555555551bc <+28>:    nop
        0x00005555555551bd <+29>:    pop    %rbp
        0x00005555555551be <+30>:    retq

    (gdb) info sym &bar
    bar in section .text of /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/link/dynamic/code/got/main
    (gdb) p/x &bar
    $1 = 0x555555555169

    # 打印ext@plt
    (gdb) x/3i 0x555555555070
        0x555555555070 <ext@plt>:    endbr64 
        0x555555555074 <ext@plt+4>:  bnd jmpq *0x2f55(%rip)        # 0x555555557fd0 <ext@got.plt>
        0x55555555507b <ext@plt+11>: nopl   0x0(%rax,%rax,1)
    # 打印ext@got.plt
    (gdb) p/x *(uint64_t *)0x555555557fd0
    $2 = 0x7ffff7fc3119
    (gdb) info sym &ext
    ext in section .text of ./libfoo.so
    (gdb) p/x &ext
    $3 = 0x7ffff7fc3119
    ```

## 延迟绑定(PLT)

由于动态链接存在GOT定位，一般比静态链接要慢一些。延迟绑定(Lazy Binding)技术就是为了提高动态链接运行时的效率而提出的。其基本思想就是当函数第一次被用到时才进行绑定(符号查找、重定位等)，如果没有用到则不进行绑定。

ELF使用PLT(Procedure Linkage Table)来实现延迟绑定。例如，我们想通过PLT跳转到`bar`函数，首先跳转到PLT中对应的结构`bar@plt`，其实现大致如下(根据GCC的版本不同，实现可能会不同)：
```asm
bar@plt:
jmp *(bar@GOT)  # 通过GOT间接跳转，如果GOT未被配置，将继续下面的语句；如果GOT已配置，跳转至bar函数
push n          # 以下代码是将bar的地址填入bar@GOT中，n是bar这个符号在重定位表".rela.plt"中的下标
push moduleID   # 模块ID
jump _dl_runtime_resolve # 通过链接器函数完成符号解析和重定位
```

### PLT相关的段
ELF将GOT拆分成两个表：

* ".got"段 - 用来保存全局变量引用的地址
* ".got.plt"段 - 用来保存外部函数引用的地址

在[例子"secondary_dep"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/secondary_dep)中，用`objdump -D libouter.so`命令打印".got"和".got.plt"段的内容如下：
```asm
Disassembly of section .got:

0000000000003fd8 <.got>:
        ...

Disassembly of section .got.plt:

0000000000004000 <_GLOBAL_OFFSET_TABLE_>:
    4000:       18 3e                   sbb    %bh,(%rsi)
        ...
    4016:       00 00                   add    %al,(%rax)
    4018:       30 10                   xor    %dl,(%rax)
    401a:       00 00                   add    %al,(%rax)
    401c:       00 00                   add    %al,(%rax)
    401e:       00 00                   add    %al,(%rax)
    4020:       40 10 00                adc    %al,(%rax)
    4023:       00 00                   add    %al,(%rax)
    4025:       00 00                   add    %al,(%rax)
    4027:       00 50 10                add    %dl,0x10(%rax)
    402a:       00 00                   add    %al,(%rax)
    402c:       00 00                   add    %al,(%rax)
        ...
```
其中，".got"段的范围是：0x3fd8-0x4000，".got.plt"段的范围是：0x4000-0x4038

* `0x3fe0`是全局变量`b`在".got"段的位置，`bar`函数中变量`b`的地址就是此位置
    ```asm
    0000000000001159 <bar>:
        1159:       f3 0f 1e fa             endbr64 
        115d:       55                      push   %rbp
        115e:       48 89 e5                mov    %rsp,%rbp
        1161:       c7 05 d1 2e 00 00 01    movl   $0x1,0x2ed1(%rip)        # 403c <a>
        1168:       00 00 00 
        116b:       48 8b 05 6e 2e 00 00    mov    0x2e6e(%rip),%rax        # 3fe0 <b>
        1172:       c7 00 02 00 00 00       movl   $0x2,(%rax)
        1178:       48 8b 05 61 2e 00 00    mov    0x2e61(%rip),%rax        # 3fe0 <b>
        117f:       8b 10                   mov    (%rax),%edx
        ...
    ```
* `0x4028`是外部函数`ext`在".got.plt"段的位置，`ext@plt`就是跳转到此位置
    ```asm
    0000000000001090 <ext@plt>:
        1090:       f3 0f 1e fa             endbr64 
        1094:       f2 ff 25 8d 2f 00 00    bnd jmpq *0x2f8d(%rip)        # 4028 <ext>
        109b:       0f 1f 44 00 00          nopl   0x0(%rax,%rax,1)
    ```

## 动态链接相关的段

在动态链接中，操作系统会先加载动态链接器(Dynamic Linker)，将控制权交给动态链接器的入口地址。当动态链接器得到控制权之后，它开始执行一系列自身的初始化操作，然后根据当前的环境参数，开始对可执行文件进行动态链接工作。当所有动态链接工作完成后，动态链接器会将控制权转交到可执行文件的入口地址，程序开始正式执行。

下面结合[例子"secondary_dep"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/secondary_dep)，介绍动态链接相关的段。

### ".interp"段

在动态链接的ELF可执行文件中，有一个专门的段叫做".interp"段，指定了动态链接器的位置。通过`objdump -s main`命令，打印".interp"段如下，
```sh
Contents of section .interp:
 0318 2f6c6962 36342f6c 642d6c69 6e75782d  /lib64/ld-linux-
 0328 7838362d 36342e73 6f2e3200           x86-64.so.2.
```
其中，`/lib64/ld-linux-x86-64.so.2`是一个软链接，指向动态链接器共享对象的位置。

### ".dynamic"段

".dynamic"段保存了动态链接器所需要的基本信息，比如依赖于哪些共享对象、动态链接器符号表的位置、动态链接重定位表的位置、共享对象初始化代码的地址等。

`readelf -d libouter.so`命令可查看libouter.so的"dynamic"段，其内容如下：
```sh
$ readelf -d libouter.so
Dynamic section at offset 0x2e08 contains 25 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libinner.so]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000c (INIT)               0x1000
 0x000000000000000d (FINI)               0x11c0
 0x0000000000000019 (INIT_ARRAY)         0x3df8
 0x000000000000001b (INIT_ARRAYSZ)       8 (bytes)
 ...
```

`ldd libouter.so`命令可查看libouter.so所依赖的动态库，其内容如下：
```sh
$ ldd libouter.so 
    linux-vdso.so.1 (0x00007fffd7fd9000)
    libinner.so => not found
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f4f1ce6b000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f4f1d07a000)
```

### 动态符号表

".dynsym"段是动态链接的动态符号表，类似于静态链接的符号表".symtab"，用于保存动态链接模块之间的符号**导入导出**关系，模块私有变量则不保存。但".symtab"段往往保存所有符号。

".dynstr"段是动态链接的动态符号字符串表，类似于静态链接的符号字符串表".strtab"段，用于保存动态链接相关符号名的字符串。同时为了加快运行时符号的查找过程，还有辅助的符号哈希表".hash"段。

命令`readelf --dyn-syms libouter.so`可查看动态符号表".dynsym"段：
```sh
$ readelf --dyn-syms libouter.so 

Symbol table '.dynsym' contains 10 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
     2: 0000000000000000     0 OBJECT  GLOBAL DEFAULT  UND b
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND printf@GLIBC_2.2.5 (2)
     4: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
     5: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND ext
     6: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
     7: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)
     8: 0000000000001159    71 FUNC    GLOBAL DEFAULT   14 bar
     9: 00000000000011a0    31 FUNC    GLOBAL DEFAULT   14 foo
```

命令`readelf -sD libouter.so`可以查看动态符号的哈希表：
```sh
$ readelf -sD libouter.so 

Symbol table of `.gnu.hash` for image:
  Num Buc:    Value          Size   Type   Bind Vis      Ndx Name
    8   0: 0000000000001159    71 FUNC    GLOBAL DEFAULT  14 bar
    9   1: 00000000000011a0    31 FUNC    GLOBAL DEFAULT  14 foo
```

### 动态链接重定位表

共享对象需要重定位的主要原因是导入符号的存在。无论是可执行文件还是共享对象，一旦依赖于其他共享对象，那么它的代码或数据中就会有对于导入符号的引用。在编译时这些导入符号的地址未知。在静态链接中，这些未知的地址引用在链接时被修正。但是在动态链接中，导入符号的地址在运行时才确定，所以需要在运行时将这些导入符号的引用修正，即需要重定位。

对于使用PIC技术的可执行文件或共享对象，虽然它们的代码段不需要重定位(因为地址无关)，但是数据段还包含了绝对地址的引用。因为代码段中绝对地址相关的部分被分离了出来，变成了GOT，而GOT实际上是数据段的一部分。除了GOT以外，数据段还可能包含绝对地址引用。

类似于静态链接中的代码段重定位表".rela.text"段和数据段的重定位表".rela.data"段，动态链接也有动态代码段重定位表".rela.plt"段和动态数据段重定位表".rela.dyn"。

* ".rela.dyn"段是对数据引用的修正，它所修正的位置位于".got"以及数据段
* ".rela.plt"段是对函数引用的修正，它所修正的位置位于".got.plt"

命令`readelf -r libouter.so`可查看重定位表：
```sh
$ readelf -r libouter.so 

Relocation section '.rela.dyn' at offset 0x4c8 contains 8 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000003df8  000000000008 R_X86_64_RELATIVE                    1150
000000003e00  000000000008 R_X86_64_RELATIVE                    1110
000000004030  000000000008 R_X86_64_RELATIVE                    4030
000000003fd8  000100000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_deregisterTMClone + 0
000000003fe0  000200000006 R_X86_64_GLOB_DAT 0000000000000000 b + 0
000000003fe8  000400000006 R_X86_64_GLOB_DAT 0000000000000000 __gmon_start__ + 0
000000003ff0  000600000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_registerTMCloneTa + 0
000000003ff8  000700000006 R_X86_64_GLOB_DAT 0000000000000000 __cxa_finalize@GLIBC_2.2.5 + 0

Relocation section '.rela.plt' at offset 0x588 contains 3 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000004018  000800000007 R_X86_64_JUMP_SLO 0000000000001159 bar + 0
000000004020  000300000007 R_X86_64_JUMP_SLO 0000000000000000 printf@GLIBC_2.2.5 + 0
000000004028  000500000007 R_X86_64_JUMP_SLO 0000000000000000 ext + 0

$ readelf -S libouter.so | grep got
  [12] .plt.got          PROGBITS         0000000000001060  00001060
  [22] .got              PROGBITS         0000000000003fd8  00002fd8
  [23] .got.plt          PROGBITS         0000000000004000  00003000
```

* R_X86_64_JUMP_SLO类型
    * 对".got.plt"段的重定位，被修正的位置只需要直接填入符号的地址
    * 以`printf`重定位为例，当动态链接器需要进行重定位时，它先查找`printf`位于`libc.so`的地址(假设为0x08801234)。那么链接器就会将这个地址填入到".got.plt"中的偏移`0x4020`的位置中。
* R_X86_64_GLOB_DAT类型
    * 对".got"端的重定位，过程和R_X86_64_JUMP_SLO类型类似
* R_X86_64_RELATIVE类型
    * 不同于前面的类型，是对绝对地址引用的重定位，又称基址重置(Rebasing)

## 动态链接的步骤和实现

动态链接的步骤基本上分三步：

* 启动动态链接器本身
* 装载所有需要的共享对象
* 重定位和初始化

### 动态链接器自举

动态链接器有两个要求：

* 动态链接器本身不可以依赖于其他任何共享对象
    * 不使用任何系统库、运行库
* 动态链接器本身所需要的全局和静态变量的重定位工作由它本身完成
    * 由一段非常精巧的代码完成，称为自举(Bootstrap)

当程序启动时，通过栈给动态链接器传递参数，下面的[代码](./code/stack/main.c)可以打印进程开始执行时栈上的信息，包括：

* 进程参数`argv`
* 环境变量`environ`
* 动态链接器所需要的一些辅助信息组(Auxiliary Vector)
    * AT_ENTRY是程序入口地址，即".text"段的起始地址，也是`_start`函数的地址

```cpp
int main(int argc, char** argv, char** environ)
{
   uint64_t* p = (uint64_t*)argv;
   printf("Argument count: %ld\n", *(p - 1));
   assert(argc == *(p - 1));

   int i = 0;
   for (; i < *(p - 1); i++)
   {
      printf("Argument %d : %s\n", i, (char *)(*(p + i)));
   }

   p += i;
   p++; // skip 0;
   assert((void*)environ == (void*)p);

   printf("Environment: \n");
   while(*p)
   {
      printf("\t%s\n", (char *)(*p));
      p++;
   }
   p++; // skip 0;

   printf("Auxiliary Vectors:\n");
   Elf64_auxv_t* aux = (Elf64_auxv_t*)p;
   while(aux->a_type != AT_NULL)
   {
      // Type is defined in /usr/include/x86_64-linux-gnu/bits/auxv.h
      printf("\tType: %02ld, Value: %lx\n", aux->a_type, aux->a_un.a_val);
      aux++;
   }

   return 0;
}
```

### 装载共享对象

完成基本自举以后，动态链接器将可执行文件和链接器本身的符号表合并到一个符号表中，我们称之为**全局符号表(Global Symbol Table)**。然后根据".dynamic"段中的`DT_NEEDED`加载所需共享对象。当一个新的共享对象被装载进来后，它的符号表会被合并到全局符号表中。

如果两个不同的模块定义了同一个符号，那么后加入全局符号表的符号会被忽略，称为共享对象的**全局符号介入(Global Symbol Interpose)**。

在[例子"sym_interpose"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/sym_interpose)中，b1.so依赖a1.so，b2.so依赖a2.so，a1.so和a2.so定义了相同的函数`a()`。动态链接器按照广度优先的顺序进行装载时，首先是main，然后是b1.so、b2.so、a1.so，最后是a2.so。因此，a2.so的`a()`函数被忽略。

### 重定位和初始化

装载完所有共享对象后，动态链接器已经拥有了进程的全局符号表，因此开始遍历可执行文件和每个共享对象的重定位表，将它们的GOT/PLT中的每个需要重定位的位置进行修正。

重定位完成后，如果某个共享对象有".init"段，那么动态链接器会执行".init"段中的代码，用以实现共享对象特有的初始化过程。比如最常见的，共享对象中的C++的全局/静态对象的构造就需要通过".init"来初始化。相应地，".finit"段会在进程退出时执行，实现全局对象析构之类的操作。

如果进程的可执行文件也有".init"段，动态链接器不会执行它，因为可执行文件的".init"段和".finit"段是由程序初始化部分代码负责执行的。

### 显式运行时链接
共享对象不仅可以通过动态链接库自动导入，也可以运行时手动导入。下面的[代码](./code/dlopen/main.c)展示了如何动态装载库(Dynamic Loading Library)：

* 动态加载`libm.so.6`共享对象
* 运行库中的`sin`函数
```cpp
int main(void)
{
   void *handle = dlopen("libm.so.6", RTLD_NOW);

   double (*func)(double);
   func = dlsym(handle, "sin");
   printf("%f\n", func(3.1415926 / 2));

   dlclose(handle);
   return 0;
}
```

当多个同名符号冲突时，`dlsym()`对符号的查找优先级分两种类型：

* “装载序列”查找
    * 当`dlopen()`第一个参数为`NULL`时，在在全局符号表中，依据“先载入符号优先”的原则进行查找
* “依赖序列”查找
    * 当`dlopen()`打开某个特定共享对象时，以当前共享对象为根节点，对它所有依赖的共享对象进行广度优先遍历查找

## Linux共享库的组织

### 共享库版本
Linux规定共享库的文件名规则如下：`libname.so.x.y.z`

* name - 库的名字
* x - 主版本号
    * 不同主版本号的库之间时不兼容的
* y - 次版本号
    * 增加一些新的接口符号，且保持原来的符号不变
* z - 发布版本号
    * 一些错误的修正、性能的改进等，并不添加任何新的接口，也不对接口进行更改

### SONAME
SONAME是共享对象的真实名字，通过`-Wl,-soname,<soname>`在编译时填入共享库，可通过命令`objdump -p <lib> | grep SONAME`或`readelf -d libfoo.so | grep SONAME`查看。

如果一个共享库存在SONAME，那么无论共享库自身的文件名如何改变，在链接时都会使用SONAME。如果一个共享库没有设置SONAME，链接时会使用文件名。如[例子"soname"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/soname)中的`libfoo.so`设置了SONAME。因此即使重命名共享库为`libbar.so`，再用`-lbar`链接，最终生成的可执行文件还是要求链接到`libfoo.so`。

在Linux系统中，SONAME用于统一标识兼容的共享库：

* SONAME只保留主版本号，规定了共享库的接口(共享库名字和主版本号可以保证兼容性)
    * 比如，一个共享库叫做`libfoo.so.2.6.1`，那么它的SO-NAME是`libfoo.so.2`
* 系统会为每各共享库在它所在的目录创建一个跟SO-NAME相同的并且指向它的软链接
    * 比如，一个共享库`/ib/libfoo.so.2.6.1`，那么存在一个软链接`/lib/libfoo.so.2`

### 链接名
GCC的提供了不同的方法指定链接的共享库：

* `-l<link_name>`参数
    * 指定需要链接的共享库`lib<link_name>.so`
* `-l:<filename>`参数
    * 通过文件名指定共享库，参考[LD手册](https://sourceware.org/binutils/docs-2.18/ld/Options.html)
* 全路径指定
* `-Wl,-static`参数
    * 指定查找静态库，通过`-Wl,-Bdynamic`恢复成动态库查找

[例子"linkname"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/linkname)提供了指定共享库的不同方法：

```makefile
# 链接当前目录下的动态库libfoo.so
main: clean foo $(OBJS)
	$(CC) $(OBJS) -L. -lfoo -o main

# 链接当前目录下的动态库libfoo.so
main2: clean foo $(OBJS)
	$(CC) $(OBJS) -L. -l:libfoo.so -o main

# 链接当前目录下的动态库libfoo.so
main3: clean foo $(OBJS)
	$(CC) $(OBJS) $(PWD)/libfoo.so -o main

# 链接当前目录下的静态库libfoo.a
main_static: clean foo_static $(OBJS)
	$(CC) $(OBJS) -L. -Wl,-static -lfoo -Wl,-Bdynamic -o main
```

## 共享库的查找过程

默认情况下，动态链接器会依次在`/etc/ld.so.cache`，`/lib`和`/user/lib`目录下寻找共享库。其中，`/etc/ld.so.cache`是由`ldconfig`工具为了加速共享库寻找过程，根据`/etc/ld.so.conf`规定的目录建立的临时缓存。

除了默认情况，Linux系统还提供了许多其他的方法，用来改变动态链接器装载共享库的路径。[例子"lib_dep"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/lib_dep/Makefile)展示了多种不同的方法，用来指定共享库的搜索路径：
```makefile
inner:
	gcc -shared -fPIC lib_inner.c -o libinner.so

outer_alone_so:
	gcc -shared -fPIC lib_outer.c -o libouter.so

outer_inner_so:
	gcc -shared -fPIC lib_outer.c -L. -linner -o libouter.so

# libouter.so does not know libinner.so
main_so: clean inner outer_alone_so
	gcc -o main main.c -L. -louter -linner
	env LD_LIBRARY_PATH=. ./main

# "-rpath" specifies the library path, so "ldd main" does not report "not found"
main_so1: clean inner outer_alone_so
	gcc -o main main.c -L. -louter -linner -Wl,-rpath=.
	env LD_LIBRARY_PATH=. ./main

# executable without libinner.so, use LD_PRELOAD to run
main_so2: clean inner outer_alone_so
	gcc -o main main.c -L. -louter -Wl,--allow-shlib-undefined
	env LD_PRELOAD=./libinner.so LD_LIBRARY_PATH=. ./main

# libouter.so knows libinner.so
main_so3: clean inner outer_inner_so
	gcc -o main main.c -L. -louter -Wl,-rpath-link=.
	env LD_LIBRARY_PATH=. ./main

# same as main_so3
main_so4: clean inner outer_inner_so
	gcc -o main main.c -L. -louter -Wl,-rpath=.
	env LD_LIBRARY_PATH=. ./main

# same as main_so3
main_so5: clean inner outer_inner_so
	gcc -o main main.c -L. -louter -Xlinker -rpath .
	env LD_LIBRARY_PATH=. ./main

# same as main_so3
main_so6: clean inner outer_inner_so
	export LD_LIBRARY_PATH=. && gcc -o main main.c -L. -louter
	env LD_LIBRARY_PATH=. ./main
```

* 编译期间 - 指定链接时的搜索路径
    * `-Wl,-rpath-link=<path>`
        * 不影响运行期间的搜索路径
    * `-Wl,-rpath=<path>`和`-Xlinker -rpath <path>`
        * `-rpath`选项会影响运行期间的搜索路径，`ldd`会从此选项指定的目录种寻找
    * `export LD_LIBRARY_PATH=<path>`
        * 不影响运行期间的搜索路径
* 运行期间
    * `export LD_LIBRARY_PATH=<path>` - 指定运行时的搜索路径
    * `LD_PRELOAD=<lib_path>` - 预先装载共享库

### 调试动态链接器

环境变量`LD_DEBUG`可以打开动态链接器的调试功能，`LD_DEBUG`可以设置成如下值：

LD_DEBUG的值 | 功能描述
--- | ---
libs | 显示共享库的搜索路径
reloc | 显示从定位过程
files | 显示处理输入文件的过程
symbols | 显示符号表查找过程
bindings | 显示动态链接的符号绑定过程
versions | 显示符号的版本依赖关系
scopes | 显示范围信息
all | 显示以上所有信息
statistics | 显示动态链接过程中的各种统计信息
unused | 显示没有使用到的共享库
help | 显示帮助信息

例如，下面的例子通过`LD_DEBUG=libs`，找到了可执行文件`main`依赖的两个共享库的位置：

* ./libfoo.so
* /lib/x86_64-linux-gnu/libc.so.6

```sh
$ env LD_DEBUG=libs LD_LIBRARY_PATH=. ./main
    438736:     find library=libfoo.so [0]; searching
    438736:      search path=./tls/haswell/x86_64:./tls/haswell:./tls/x86_64:./tls:./haswell/x86_64:./haswell:./x86_64:.           (LD_LIBRARY_PATH)
    438736:       trying file=./tls/haswell/x86_64/libfoo.so
    438736:       trying file=./tls/haswell/libfoo.so
    438736:       trying file=./tls/x86_64/libfoo.so
    438736:       trying file=./tls/libfoo.so
    438736:       trying file=./haswell/x86_64/libfoo.so
    438736:       trying file=./haswell/libfoo.so
    438736:       trying file=./x86_64/libfoo.so
    438736:       trying file=./libfoo.so
    438736:
    438736:     find library=libc.so.6 [0]; searching
    438736:      search path=./tls/haswell/x86_64:./tls/haswell:./tls/x86_64:./tls:./haswell/x86_64:./haswell:./x86_64:.           (LD_LIBRARY_PATH)
    438736:       trying file=./tls/haswell/x86_64/libc.so.6
    438736:       trying file=./tls/haswell/libc.so.6
    438736:       trying file=./tls/x86_64/libc.so.6
    438736:       trying file=./tls/libc.so.6
    438736:       trying file=./haswell/x86_64/libc.so.6
    438736:       trying file=./haswell/libc.so.6
    438736:       trying file=./x86_64/libc.so.6
    438736:       trying file=./libc.so.6
    438736:      search cache=/etc/ld.so.cache
    438736:       trying file=/lib/x86_64-linux-gnu/libc.so.6
    ...
```
