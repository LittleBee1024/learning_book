# 静态链接

> [《程序员的自我修养--链接、装载与库》 - 俞甲子，石凡，潘爱民](https://1drv.ms/b/s!AkcJSyT7tq80cUuHb2eRcJkkBjM?e=YUwBqB)，第四章的读书笔记；以及[《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG)，第7.6.3章节的读书笔记。本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/static/code)中找到

## 静态链接过程

链接器采用相似段合并的方式，将相同性质的段合并到一起。整个链接过程分两步：

* 空间与地址分配
    * 扫描所有输入文件，创建全局符号表，并合并输入文件
* 符号解析与重定位
    * 读取段的数据和重定位信息，进行符号解析与重定位、调整代码中的地址

### 空间与地址分配

以[a.c](./code/alloc_reloc/a.c)和[b.c](./code/alloc_reloc/b.c)为例，通过`ld a.o b.o -e main -o main`链接。链接前后的地址分配情况如下：
```asm
a.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000023  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000000  0000000000000000  0000000000000000  00000063  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  ...

b.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000010  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000004  0000000000000000  0000000000000000  00000050  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  ...

main:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .note.gnu.property 00000020  00000000004001c8  00000000004001c8  000001c8  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 .text         00000033  0000000000401000  0000000000401000  00001000  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  2 .eh_frame     00000058  0000000000402000  0000000000402000  00002000  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  3 .data         00000004  0000000000404000  0000000000404000  00003000  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  ...
```

VMA标识`Virtual Memory Address`，即虚拟地址。在链接之前，目标文件中的所有段的VMA都是0，因为虚拟空间还没有被分配。等到链接之后，可执行文件中的各个段都被分配到了相应的虚拟地址。其中，`.text`段在可执行文件中被分配到了地址是`0x0000000000401000`，大小为0x29字节，是a.o和b.o的`.text`段大小之和。

### 符号解析与重定位

* 重定位之前的`main`函数
```asm hl_lines="5 8"
0000000000000000 <main>:
   0:   f3 0f 1e fa             endbr64 
   4:   55                      push   %rbp
   5:   48 89 e5                mov    %rsp,%rbp
   8:   c7 05 00 00 00 00 02    movl   $0x2,0x0(%rip)           # 12 <main+0x12>，重定位shared变量前
   f:   00 00 00 
  12:   b8 00 00 00 00          mov    $0x0,%eax
  17:   e8 00 00 00 00          callq  1c <main+0x1c>           # 重定位swap函数前
  1c:   b8 00 00 00 00          mov    $0x0,%eax
  21:   5d                      pop    %rbp
  22:   c3                      retq
```

* 重定位之后的`main`函数
```asm hl_lines="5 8"
0000000000401000 <main>:
  401000:       f3 0f 1e fa             endbr64 
  401004:       55                      push   %rbp
  401005:       48 89 e5                mov    %rsp,%rbp
  401008:       c7 05 ee 2f 00 00 02    movl   $0x2,0x2fee(%rip)  # 404000 <shared>，重定位shared变量后
  40100f:       00 00 00 
  401012:       b8 00 00 00 00          mov    $0x0,%eax
  401017:       e8 07 00 00 00          callq  401023 <swap>      # 重定位swap函数后
  40101c:       b8 00 00 00 00          mov    $0x0,%eax
  401021:       5d                      pop    %rbp
  401022:       c3                      retq
```

#### 重定位表
重定位表(Relocation Table)用来保存于重定位相关的信息。每个要被重定位的ELF段都有一个对应的重定位段。例如，如果代码段`.text`有要被重定位的地方，那么会有一个相应叫`.rela.text`的段保存了代码段的重定位表；如果数据段`.data`有要被重定位的地方，就会有一个叫`.rela.data`的段保存数据段的重定位表。

可以使用`objdump -r`或者`readelf -r`查看重定位表的内容：
```asm
> readelf -r a.o

Relocation section '.rela.text' at offset 0x250 contains 2 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
00000000000a  000a00000002 R_X86_64_PC32     0000000000000000 shared - 8
000000000018  000c00000004 R_X86_64_PLT32    0000000000000000 swap - 4
```

每个要被重定位的地方叫一个**重定位入口**(Relocation Entry)。重定位入口的偏移(Offset)表示该入口在要被重定位的段中的位置。例如，`shared`在`a.o`的`.text`段的`0xa`的位置。

#### 符号解析
重定位的过程中，链接器会查找由所有输入目标文件的符号表组成的**全局符号表**，找到相应的符号的目标地址，然后进程重定位。

通过`readelf -s`可以查看符号表：
```asm
> readelf -s main

Symbol table '.symtab' contains 14 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 00000000004001c8     0 SECTION LOCAL  DEFAULT    1 
     2: 0000000000401000     0 SECTION LOCAL  DEFAULT    2 
     3: 0000000000402000     0 SECTION LOCAL  DEFAULT    3 
     4: 0000000000404000     0 SECTION LOCAL  DEFAULT    4 
     5: 0000000000000000     0 SECTION LOCAL  DEFAULT    5 
     6: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS a.c
     7: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS b.c
     8: 0000000000401023    16 FUNC    GLOBAL DEFAULT    2 swap
     9: 0000000000404000     4 OBJECT  GLOBAL DEFAULT    4 shared
    10: 0000000000404004     0 NOTYPE  GLOBAL DEFAULT    4 __bss_start
    11: 0000000000401000    35 FUNC    GLOBAL DEFAULT    2 main
    12: 0000000000404004     0 NOTYPE  GLOBAL DEFAULT    4 _edata
    13: 0000000000404008     0 NOTYPE  GLOBAL DEFAULT    4 _end
```

结合全局符号表和重定位类型，链接器就可以完成重定位工作了。例如，`shared`在全局符号表中的地址是`0x404000`。`shared`的重定位类型是相对寻址修正类型`R_X86_64_PC32`。因此，在最终的可执行文件中，`shared`的地址被修正为`0x2fee`，其值正好等于：`0x404000 - 0x401012`。
```asm
  401008:       c7 05 ee 2f 00 00 02    movl   $0x2,0x2fee(%rip)        # 404000 <shared>
  40100f:       00 00 00 
  401012:       b8 00 00 00 00          mov    $0x0,%eax
```

## C++链接问题
### 重复代码消除
C++编译器在很多时候会产生重复的代码，比如模板(Template)，外部内联函数(Extern Inline Function)和虚函数表(Virtual Function Table)都有可能在不同的编译单元里生成相同的代码。

例如，当一个模板在多个编译单元同时实例化成相同的类型的时候，会生成重复的代码。编译器会将每个模板的实例代码都单独存放在一个段里，每个段只包含一个模板实例。链接器在最终链接的时候可以区分这些相同的模板实例段，然后将他们合并入最后的代码段。

同样，定义在头文件中的类方法，会出现在多个编译单元中，但在最终链接时被消除合并。在[例子"cpp_link"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/static/code/)中，`foo.h`中定义了`Foo::foo()`方法：
```cpp
class Foo
{
public:
   void foo()
   {
      printf("foo\n");
   }
   void bar();
};
```
编译后，在`obj1.o`，`obj2.o`，`main.o`三个编译单元中都存在相同的段`.text._ZN3Foo3foo`，并且`_ZN3Foo3fooEv`符号类型都是弱类型`WEAK`。但是在最终的可执行文件`main`中，`_ZN3Foo3fooEv`被合并到`.text`段中。
```bash
> readelf -S obj1.o
...
[ 7] .text._ZN3Foo3foo PROGBITS         0000000000000000  0000006a
       0000000000000022  0000000000000000 AXG       0     0     2
...
> readelf -s obj1.o
...
12: 0000000000000000    34 FUNC    WEAK   DEFAULT    7 _ZN3Foo3fooEv
...
> readelf -s main
...
70: 0000000000001186    34 FUNC    WEAK   DEFAULT   16 _ZN3Foo3fooEv
...
> objdump -D main
...
0000000000001186 <_ZN3Foo3fooEv>:
    1186:       f3 0f 1e fa             endbr64
    118a:       55                      push   %rbp
    118b:       48 89 e5                mov    %rsp,%rbp
    118e:       48 83 ec 10             sub    $0x10,%rsp
    1192:       48 89 7d f8             mov    %rdi,-0x8(%rbp)
    1196:       48 8d 05 6b 0e 00 00    lea    0xe6b(%rip),%rax        # 2008 <_IO_stdin_used+0x8>
    119d:       48 89 c7                mov    %rax,%rdi
    11a0:       e8 ab fe ff ff          callq  1050 <puts@plt>
    11a5:       90                      nop
    11a6:       c9                      leaveq
    11a7:       c3                      retq
...
```

### 全局构造与析构

Linux系统下一般的程序入口是`_start`，在完成一系列初始化过程后，再调用`main`函数来执行程序的主体。`main`函数执行完成后，返回到初始化部分，进行一些清理工作。相关初始化代码定义在下面的两个段内：

* .init段
    * 初始化代码，在main函数之前调用
* .fini段
    * 进程终止代码，在main函数退出时执行

C++的全局对象的构造和析构函数，就利用了此初始化过程。

## 链接过程控制

### 链接控制脚本
可通过三种方法控制链接过程：

* 使用命令行来给链接器指定参数
* 将链接指令存放在目标文件里面
    * 如，`.drectve`段
* 使用链接控制脚本
    * `ld -verbose`命令可查看默认链接脚本
    * `ld -T <link.script>`可指定链接脚本

### 最小的程序
[例子"tiny_hello_32"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/static/code/tiny_hello_32)和[例子"tiny_hello_64"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/static/code/tiny_hello_64)分别是32位和64位下，最小的"Hello World"程序。此程序和普通程序有以下不同：

* 没有用C语言库，如`printf`等函数。利用汇编语言直接实现系统调用。
* 不以`_start`作为程序入口，直接以用户定义的`nomain`函数作为程序入口。
* 可通过链接脚本将所有段合并到自定义的`tinytext`段

以64位为例，源代码如下：
```cpp
char* str = "Hello World\n";

void print()
{
   // int write(int fd, char* buffer, int size);
   // write(rdi, rsi, rdx);
   //    fd = stdout (0)
   //    buffer = str
   //    size = 13
   // WRITE system call ID = 1 (rax)
   asm(
      "movq $13,%%rdx \n\t"
      "movq %0,%%rsi \n\t"
      "movq $0,%%rdi \n\t"
      "movq $1,%%rax \n\t"
      "syscall \n\t"
      ::"r"(str):"rdi","rsi","rdx"
   );
}

void exit()
{
   // Syscall 60 is exit on x86_64
   // __asm ("movq $60,%rax; movq $42,%rdi; syscall");
   asm(
      "movq $42,%rdi \n\t"
      "movq $60,%rax \n\t"
      "syscall \n\t"
   );
}

void nomain()
{
   print();
   exit();
}
```

链接脚本`main.lds`如下，指定了入口函数和段信息：
```cpp
ENTRY(nomain)

SECTIONS
{
   . = 0x400000 + SIZEOF_HEADERS;
   tinytext : { *(.text) *(.data) *(.rodata) }
   /DISCARD/ : { *(.comment) }
}
```

通过`ld -static -T main.lds -o main main.o`命令，可生成可执行文件`main`。脚本中的`SIZEOF_HEADERS`等于`0x120`，所以`.note.gnu.property`段的VMA等于`0x400120`。通过GDB打印运行时的地址空间，发现进程`main`被加载到`0x400000~0x401000`页，和链接脚本一致。
```asm
> objdump -h main

main:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .note.gnu.property 00000020  0000000000400120  0000000000400120  00000120  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 .eh_frame     00000078  0000000000400140  0000000000400140  00000140  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  2 tinytext      00000073  00000000004001b8  00000000004001b8  000001b8  2**0
                  CONTENTS, ALLOC, LOAD, CODE
  3 .data.rel.local 00000008  0000000000400230  0000000000400230  00000230  2**3
                  CONTENTS, ALLOC, LOAD, DATA

> make debug
Mapped address spaces:

          Start Addr           End Addr       Size     Offset objfile
            0x400000           0x401000     0x1000        0x0 /home/yuxiangw/GitHub/learning_book/docs/booknotes/cxydzwxy/link/static/code/tiny_hello_64/main
      0x7ffff7ff9000     0x7ffff7ffd000     0x4000        0x0 [vvar]
      0x7ffff7ffd000     0x7ffff7fff000     0x2000        0x0 [vdso]
      0x7ffffffde000     0x7ffffffff000    0x21000        0x0 [stack]
  0xffffffffff600000 0xffffffffff601000     0x1000        0x0 [vsyscall]
```

### ld链接脚本语法

链接脚本的语句分两种：

* 赋值语句
    * 如`. = 0x400000 + SIZEOF_HEADERS;`，指定了SECTION的起始地址
* 命令语句
    * 如`ENTRY(nomain)`，指定了进程的入口地址

命令语句 | 说明
--- | ---
ENTRY(symbol) | 按照以下优先级顺序设置进程入口地址：<br /> 1. ld命令行的-e选项 <br /> 2. 链接脚本的ENTRY(symbol)命令 <br /> 3. 如果定义了_start符号，使用_start符号值 <br /> 4. 如果存在.text段，使用.text段的第一字节的地址 <br /> 5. 使用值0
STARTUP(filename) | 将文件`filename`作为链接过程中的第一个输入文件
SEARCH_DIR(path) | 将路径path加入到ld链接器的库查找目录
INPUT(file, file, ...) | 指定文件作为链接过程中的输入文件
INCLUDE filename | 将指定文件包含进链接脚本
PROVIDE(symbol) | 在链接脚本中定义某个符号
SECTIONS{...} | 组织段内容

SECTIONS命令最基本格式如下：
```cpp
SECTIONS
{
    ...
    secname : { contents }
    ...
}
```

* secname表示输出段的段名，后面必须有一个空格符
    * `/DISCARD/`表示特殊的输出段，此输出段的内容将被丢弃
* contents描述了一套规则，符号此规则的输入段会合并到输出段
    * 例如，`*(.text) *(.data) *(.rodata)`表示将所有`.o`文件的`.text`段，`.data`段和`.rodata`段都合并到输出段
