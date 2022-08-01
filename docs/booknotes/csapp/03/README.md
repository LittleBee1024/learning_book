# 程序的机器表示

> [《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG)，第三章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/03/code)中找到

## 程序编码

机器代码将对C语言程序员隐藏的处理器状态都变为可见，如：

* 程序计数器
    * 将要执行的下一条指令在内存中的地址
* 整数寄存器文件
    * 16个命名的位置，分别存储64位的值
* 条件码寄存器
    * 保持最近执行的算术或逻辑指令的状态信息
* 一组向量寄存器
    * 存放一共或多个整数或浮点数值，用于向量运算

[例子"mstore"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/03/code/mstore)以[`-Og`编译器优化选项](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)，得到`multstore`对应的汇编代码如下：

=== "链接前的汇编代码"

    ```asm
    # "objdump -d mstore.o"得到的反汇编信息
    # void multstore(long x, long y, long *dest)
    # x in %rdi, y in %rsi, dest in %rdx
    0000000000000000 <multstore>:
     0:   f3 0f 1e fa             endbr64
     4:   53                      push   %rbx               # 保存 %rbx
     5:   48 89 d3                mov    %rdx,%rbx          # 拷贝 dest 到 %rbx
     8:   e8 00 00 00 00          callq  d <multstore+0xd>  # 调用子函数
     d:   48 89 03                mov    %rax,(%rbx)        # 保存子函数返回值到 *dest
    10:   5b                      pop    %rbx               # 恢复 %rbx
    11:   c3                      retq 
    ```

=== "链接后的汇编代码"

    ```asm hl_lines="6"
    # "objdump -d main"得到的反汇编信息
    00000000000011d5 <multstore>:
        11d5:       f3 0f 1e fa             endbr64 
        11d9:       53                      push   %rbx
        11da:       48 89 d3                mov    %rdx,%rbx
        11dd:       e8 e7 ff ff ff          callq  11c9 <mult2>
        11e2:       48 89 03                mov    %rax,(%rbx)
        11e5:       5b                      pop    %rbx
        11e6:       c3                      retq   
        11e7:       66 0f 1f 84 00 00 00    nopw   0x0(%rax,%rax,1)
        11ee:       00 00
    ```

=== "C语言代码"

    ```cpp
    void multstore(long x, long y, long *dest)
    {
        long t = mult2(x, y);
        *dest = t;
    }
    ```

## 数据格式

| C声明 | Intel数据类型 | 汇编代码后缀 | 大小(字节) |
| --- | --- | --- | --- |
| char | 字节 | b | 1 |
| short | 字 | w | 2 |
| int | 双字 | l | 4 |
| long | 四字 | q | 8 |
| char* | 四字 | q | 8 |
| float | 单精度 | s | 4 |
| double | 双精度 | l | 8 |

如上表所示，大多数GCC生成的汇编代码指令都有一个字符的后缀，表明操作数的大小。例如，数据传送指令有四个变种：

* `movb`(传送字节，byte)
* `movw`(传送字，word)
* `movl`(传送双字，long word)
* `movq`(传送四字，quad word)。


## 访问信息

一个x86-64的中央处理器包含一组16个存储64位值的**通用目的寄存器**。

* 最初的8086中有8个16位的寄存器，即下图中的`%ax`到`%sp`
* 扩展到IA32架构时，这些寄存器也扩展成64位，标号从`%eax`到`%esp`
* 扩展到x86-64后，原来的8个寄存器扩展成64位，标号从`%rax`到`%rsp`，除此之外，还新增了8个寄存器，标号从`%r8`到`%r15`

![registers_64bit](./images/registers_64bit.png)

指令可以对寄存器的低位字节中存放的不同大小的数据进行操作。当对寄存器部分字节进行操作时，其他字节遵循两条规则：

* 操作1字节和2字节的指令会保存剩下的字节不变
* 操作4字节的指令会把高位4个字节置为0

例如，下面的例子对不同字节位写入`-1`，会产生不同的结果：
```asm
movabsq $0011223344556677, %rax     # %rax = 0011223344556677
movb    $-1, %al                    # %rax = 00112233445566FF
movw    $-1, %ax                    # %rax = 001122334455FFFF
movl    $-1, %eax                   # %rax = 00000000FFFFFFFF
movq    $-1, %rax                   # %rax = FFFFFFFFFFFFFFFF
```

在常见的程序里，不同的寄存器扮演着不同的角色。例如，

* `%rsp`始终指向栈顶
* `%rbp`指向当前栈开始的地方
* 函数调用时，前6个参数依次存放在以下寄存器中
    * `%rdi`，`%rsi`，`%rdx`，`%rcx`，`%r8`，`%r9`
* `%rax`用户存放函数的返回值

更详细的介绍，可参考本文的["过程"](#_7)章节。

### 操作数指示符

x86-64支持多种操作数格式。源数据值可以是常数、或是寄存器或内存中的值。目的数据可以存放在寄存器或内存中。各种不同的操作数被分为三种类型：

* 立即数`Imm`
* 寄存器`R[ra]`
* 内存引用`M[Imm]`
    * 存在多种寻找方式

| 类型 | 格式 | 操作数值 | 名称 |
| --- | --- | --- | --- |
| 立即数 | $Imm | Imm | 立即数寻址 |
| 寄存啊 | ra | R[ra] | 寄存器寻址 |
| 存储器 | Imm | M[Imm] | 绝对寻址 |
| 存储器 | (ra) | M[R[ra]] | 间接寻址 |
| 存储器 | Imm(rb) | M[Imm + R[rb]] | (基址+偏移量)寻址 |
| 存储器 | (rb,ri) | M[R[rb]+ R[ri]] | 变址寻址 |
| 存储器 | Imm(rb,ri) | M[Imm + R[rb]+ R[ri]] | 变址寻址 |
| 存储器 | (,ri,s) | M[R[ri] * s] | 比例变址寻址 |
| 存储器 | Imm(,ri,s) | M[Imm + R[ri] * s] | 比例变址寻址 |
| 存储器 | (rb,ri,s) | M[R[rb]+ R[ri] * s] | 比例变址寻址 |
| 存储器 | Imm(rb,ri,s) | M[Imm + R[rb]+ R[ri] * s] | 比例变址寻址 |

### 数据传送指令

| 指令 | 描述 | 例子 |
| --- | --- | --- |
| mov S, D | 把数据`S`传送给`D` | movb, movw, movl, movq |
| movabsq I, R | 把四字立即数存储在寄存器`R`中 | `movabsq $0011223344556677, %rax` |
| movz S, R | 把数据`S`存储在寄存器`R`中，高位用零补齐 | movzbw, movzbl, movzwl, movzbq, movzwq |
| movs S, R | 把数据`S`存储在寄存器`R`中，高位用符号位补齐 | movsbw, movsbl, movswl, movsbq, movswq, movslq |
| cltq | 把`%eax`符号扩展到`%rax` | |

例如，下面的例子描述了不同传送指令的效果：
```asm
movabsq $0011223344556677, %rax     # %rax = 0011223344556677
movb    $0xAA, %dl                  # %dl  = AA
movb    %dl, %al                    # %rax = 00112233445566AA
movsbq  $dl, %rax                   # %rax = FFFFFFFFFFFFFFAA
movzbq  $dl, %rax                   # %rax = 00000000000000AA
```

[例子"mov"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/03/code/asm_access/mov)中的`exchange`函数利用了`mov`指令，修改了指针`xp`指向的内容：

=== "ASM"

    ```asm
    # long exchange(long *xp, long y)
    # xp in %rdi, y in %rsi
    0000000000000000 <exchange>:
    0:   f3 0f 1e fa             endbr64
    4:   48 8b 07                mov    (%rdi),%rax     # 从地址是 %rdi 的内存中获取值(*xp)，并传递到返回值 %rax 中
    7:   48 89 37                mov    %rsi,(%rdi)     # (*xp = y)
    a:   c3                      retq
    ```

=== "C"

    ```cpp
    long exchange(long *xp, long y)
    {
        long x = *xp;
        *xp = y;
        return x;
    }
    ```

### 压入和弹出栈数据

| 指令 | 效果 | 描述 |
| --- | --- | --- |
| pushq S | R[%rsp] <- R[%rsp] - 8; M[R[%rsp]] <- S | 将四字压入栈 |
| popq D | D <- M[R[%rsp]]; R[%rsp] <- R[%rsp] + 8 | 将四字弹出栈 |

`pushq`和`popq`指令只有一个操作数 --- 压入的数据源和弹出的数据目的。

因为栈和程序代码都放在同以内存中，所以程序可以用标准的内存寻址方式访问栈内的任意位置。例如，假设栈顶元素是四字，指令`movq 8(%rsp), %rdx`会将第二个四字从栈中复制到寄存器`%rdx`中。

## 算术和逻辑操作

| 指令 | 效果 | 描述 |
| --- | --- | --- |
| leaq S,R | R ← &S    | 加载有效地址，目的地必须是寄存器 |
| inc  D   | D ← D+1   | 加1，可以是寄存器，也可以是一个内存位置 |
| dec  D   | D ← D−1   | 减1 |
| neg  D   | D ← -D    | 取负 |
| not  D   | D ← ~D    | 取补 |
| add  S,D | D ← D + S | 加，如果是写回内存的操作，第一个操作数必须是从内存读出 |
| sub  S,D | D ← D − S | 减 |
| imul S,D | D ← D ∗ S | 乘 |
| xor  S,D | D ← D ^ S | 异或 |
| or   S,D | D ← D | S | 或 |
| and  S,D | D ← D & S | 与 |
| sal  k,D | D ← D<<k  | 左移，移位量可以是立即数，或单字节寄存器`%cl` |
| shl  k,D | D ← D<<k  | 左移 (等同SAL) |
| sar  k,D | D ← D>>k  | 算术右移 |
| shr  k,D | D ← D>>k  | 逻辑右移 |

### 加载有效地址

加载有效地址(load effective address)指令`leaq`实际上是`movq`指令的变形，将有效地址写入目的操作数（必须是寄存器）。

[例子"lea"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/03/code/asm_operate/lea)中的`scale`函数利用`leaq`指令，对`x + 4 * y + 12 * z`表达进行了求值：

=== "ASM"

    ```asm
    # long scale(long x, long y, long z)
    # x in %rdi, y in %rsi, z in %rdx
    0000000000000000 <scale>:
     0:   f3 0f 1e fa             endbr64 
     4:   48 8d 04 b7             lea    (%rdi,%rsi,4),%rax    # x + 4*y
     8:   48 8d 14 52             lea    (%rdx,%rdx,2),%rdx    # z + 2*z = 3*z
     c:   48 8d 04 90             lea    (%rax,%rdx,4),%rax    # (x+4*y)+4*(3*z) = x + 4*y + 12*z
    10:   c3                      retq
    ```

=== "C"

    ```cpp
    long scale(long x, long y, long z)
    {
        long t = x + 4 * y + 12 * z;
        return t;
    }
    ```

### 移位操作

[例子"shl"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/03/code/asm_operate/shl)中的`arith`函数对常见算术表达进行了求值：

=== "ASM"

    ```asm
    # long arith(long x, long y, long z)
    # x in %rdi, y in %rsi, z in %rdx
    0000000000000000 <arith>:
     0:   f3 0f 1e fa             endbr64 
     4:   48 31 f7                xor    %rsi,%rdi              # t1 = x ^ y
     7:   48 8d 04 52             lea    (%rdx,%rdx,2),%rax     # 3*z
     b:   48 c1 e0 04             shl    $0x4,%rax              # t2 = 16 * (3*z) = 48*z
     f:   81 e7 0f 0f 0f 0f       and    $0xf0f0f0f,%edi        # t3 = t1 & 0x0F0F0F0F
    15:   48 29 f8                sub    %rdi,%rax              # Return t2 - t3
    18:   c3                      retq
    ```

=== "C"

    ```cpp
    long arith(long x, long y, long z)
    {
        long t1 = x ^ y;
        long t2 = z * 48;
        long t3 = t1 & 0x0F0F0F0F;
        long t4 = t2 - t3;
        return t4;
    }
    ```

## 控制

## 过程
