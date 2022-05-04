# 静态链接

> [《程序员的自我修养--链接、装载与库》 - 俞甲子，石凡，潘爱民](https://1drv.ms/b/s!AkcJSyT7tq80cUuHb2eRcJkkBjM?e=YUwBqB)，第四章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/link/staic/code)中找到

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
```asm
0000000000000000 <main>:
   0:   f3 0f 1e fa             endbr64 
   4:   55                      push   %rbp
   5:   48 89 e5                mov    %rsp,%rbp
   8:   c7 05 00 00 00 00 02    movl   $0x2,0x0(%rip)        # 12 <main+0x12>
   f:   00 00 00 
  12:   b8 00 00 00 00          mov    $0x0,%eax
  17:   e8 00 00 00 00          callq  1c <main+0x1c>
  1c:   b8 00 00 00 00          mov    $0x0,%eax
  21:   5d                      pop    %rbp
  22:   c3                      retq
```

* 重定位之后的`main`函数
```asm
0000000000401000 <main>:
  401000:       f3 0f 1e fa             endbr64 
  401004:       55                      push   %rbp
  401005:       48 89 e5                mov    %rsp,%rbp
  401008:       c7 05 ee 2f 00 00 02    movl   $0x2,0x2fee(%rip)        # 404000 <shared>
  40100f:       00 00 00 
  401012:       b8 00 00 00 00          mov    $0x0,%eax
  401017:       e8 07 00 00 00          callq  401023 <swap>
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
