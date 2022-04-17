# 动态链接

> [《程序员的自我修养--链接、装载与库》 - 俞甲子，石凡，潘爱民](https://1drv.ms/b/s!AkcJSyT7tq80cUuHb2eRcJkkBjM?e=YUwBqB)，第七章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code)中找到

## 基本实现
在Linux系统中，ELF动态链接文件被称为动态共享对象(DSO, Dynamic Shared Objects)，简称**共享对象**，它们一般都是以".so"为扩展名的一些文件。

程序和动态链接库之间的链接工作是由动态链接器完成的。不同于静态链接器，动态链接器把链接过程从程序装载前推迟到了装载的时候。这种方法称为延迟绑定(Lazy Binding)，虽然损失了一些运行时的性能，但是提高了灵活性。

### 简单的例子
[示例“simple”](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/dynamic/code/simple)中的两个可执行文件`program1`和`program2`都动态链接到了动态共享对象`libfoo.so`。当程序模块`program1.c`被编译成`program1.o`时，编译器不知道`foobar()`函数的地址。当链接器将`program1.o`链接成可执行文件时，通过函数符号信息，链接器可判断`foobar()`的性质，

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

我们把共享对象模块中的地址引用按照是否跨模块分成：模块内部引用和模块外部引用，按照不同的引用方式又可分为指令引用和数据访问，如[代码](./code/fPIC/lib_outer.c)所示的四种情况：
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

* 模块内数据访问`a`和模块间数据访问`b`
    * 对于模块内数据访问，直接用相对地址即可得到存储位置
    * 对于模块间数据访问，需要全局偏移表(Global Offset Table, GOT)的帮助。例如，`b`的GOT地址是`0xf7fc102c`，该地址指向`b`真正的地址，并且在装载时才能确定
    ```asm
    (gdb) disas bar
    Dump of assembler code for function bar:
        0x00007ffff7fc3159 <+0>:     endbr64 
        0x00007ffff7fc315d <+4>:     push   %rbp
        0x00007ffff7fc315e <+5>:     mov    %rsp,%rbp
        0x00007ffff7fc3161 <+8>:     movl   $0x1,0x2ed1(%rip)        # 0x7ffff7fc603c <a>
        0x00007ffff7fc316b <+18>:    mov    0x2e6e(%rip),%rax        # 0x7ffff7fc5fe0
        0x00007ffff7fc3172 <+25>:    movl   $0x2,(%rax)

    # 打印a变量地址0x7ffff7fc603c对应的内容
    (gdb) p/x *0x7ffff7fc603c
    $1 = 0x1

    # 打印b变量地址0x7ffff7fc5fe0对应的内容
    (gdb) p/x *0x7ffff7fc5fe0
    $2 = 0xf7fc102c
    ```
* 模块内调用`bar`和模块间调用`ext`
    * 不同于书中的描述，在GCC-11中，模块内部调用和模块间调用并无差别，都利用了[延迟绑定PLT](#plt)技术
    ```asm
    (gdb) disas foo
    Dump of assembler code for function foo:
        0x00007ffff7fc31a0 <+0>:     endbr64 
        0x00007ffff7fc31a4 <+4>:     push   %rbp
        0x00007ffff7fc31a5 <+5>:     mov    %rsp,%rbp
        0x00007ffff7fc31a8 <+8>:     mov    $0x0,%eax
        0x00007ffff7fc31ad <+13>:    callq  0x7ffff7fc3090 <ext@plt>
        0x00007ffff7fc31b2 <+18>:    mov    $0x0,%eax
        0x00007ffff7fc31b7 <+23>:    callq  0x7ffff7fc3070 <bar@plt>
        0x00007ffff7fc31bc <+28>:    nop
        0x00007ffff7fc31bd <+29>:    pop    %rbp
        0x00007ffff7fc31be <+30>:    retq
    ```

## 延迟绑定(PLT)

由于动态链接存在GOT定位，一般比静态链接要慢一些。延迟绑定(Lazy Binding)技术就是为了提高动态链接运行时的效率而提出的。其基本思想就是当函数第一次被用到时才进行绑定(符号查找、重定位等)，如果没有用到则不进行绑定。

ELF使用PLT(Procedure Linkage Table)来实现延迟绑定。例如，我们想通过PLT跳转到`bar`函数，首先跳转到PLT中对应的结构`bar@plt`，其实现大致如下(根据GCC的版本不同，实现可能会不同)：
```asm
bar@plt:
jmp *(bar@GOT)  # 通过GOT间接跳转，如果GOT未被配置，将继续下面的语句；如果GOT已配置，跳转至bar函数
push n          # 以下代码是将bar的地址填入bar@GOT中，n是bar这个符号在重定位表".rel.plt"中的下标
push moduleID   # 模块ID
jump _dl_runtime_resolve # 通过链接器函数完成符号解析和重定位
```

### PLT相关的段
ELF将GOT拆分成两个表：

* ".got"段 - 用来保存全局变量引用的地址
* ".got.plt"段 - 用来保存外部函数引用的地址

在["fPIC例子"](./code/fPIC/lib_outer.c)中，用`objdump -D libouter.so`命令打印".got"和".got.plt"段的内容如下：
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



### 动态链接重定位表


