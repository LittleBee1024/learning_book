# 调试前的必知必会

> [《Debug Hacks--深入调试的技术和工具》 - 吉冈弘隆](https://1drv.ms/b/s!AkcJSyT7tq80clf1-pjOCricrUs?e=xIJL0b)，第二章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/debug_hacks/basic/code)中找到

## x84_64寄存器

### 通用寄存器的用途

[x86_64](http://6.s081.scripts.mit.edu/sp18/x86-64-architecture-guide.html)定义了16各通用寄存器，它们的用途如下表所示：

Register | Purpose                                        | Saved across calls
---      | ---                                            | ---
%rax     | temp register; (保存函数返回值)                 | No
%rbx     | callee-saved                                   | Yes
%rcx     | used to pass 4th argument to functions         | No
%rdx     | used to pass 3rd argument to functions         | No
%rsp     | stack pointer （指向栈顶）                      | Yes
%rbp     | callee-saved; base pointer（指向栈帧开始的地方） | Yes
%rsi     | used to pass 2nd argument to functions         | No
%rdi     | used to pass 1st argument to functions         | No
%r8      | used to pass 5th argument to functions         | No
%r9      | used to pass 6th argument to functions         | No
%r10-r11 | temporary                                      | No
%r12-r15 | callee-saved registers                         | Yes

### 常见操作符

操作类型 | 格式      | 操作数值
---     | ---       | ---
立即数   | `$Imm`    | `Imm`，立即数寻址
寄存器   | `Ea`      | `R[Ea]`，寄存器寻址
内存     | `Imm`     | `M[Imm]`，绝对寻址
内存     | `(Ea)`    | `M[R[Ea]]`，间接寻址
内存     | `Imm(Ea)` | `M[Imm + R[Ea]]`，（基地址+偏移量）寻址

* `Imm`表示立即数，例如：0x8048d8e
* `Ex`表示寄存器，例如：%rax
* `R[Ex]`表示寄存器Ex的值
* `M[x]`表示地址为x的内存中的值

## 栈

> 本节通过调试器[GDB](../../../demos/gdb/README.md)的帮助，来了解栈在程序运行中的作用

### 函数调用和栈的关系

在函数调用的过程中，栈上依次保存了：

* 传给函数的参数
* 调用者的返回地址
* 上层栈帧指针`%rbp`的值
* 函数内部使用的自动变量

![frame](./images/frame.svg)

每个函数都拥有自己的栈帧(stack frame)，栈帧的起始地址由`%rbp`帧指针保存，栈顶由`%rsp`栈指针保存。

以["sum.c"](./code/stack/sum.c)代码为例子，`main`函数(caller)调用了`sum_till_MAX`函数(callee)，其相关汇编代码如下：

* `main`函数调用处的汇编代码
    ```asm
    0x0000000000001296 <+180>:   mov    $0x0,%edi               # 初始化参数1的值为0
    0x000000000000129b <+185>:   callq  0x11a9 <sum_till_MAX>   # 跳转至sum_till_MAX，其地址为0x11a9
    0x00000000000012a0 <+190>:   mov    %rax,-0x8(%rbp)         # 将sum_till_MAX返回的结果存到：M[%rbp-0x8]
    ```

* `sum_till_MAX`函数的汇编代码
    ```asm
    0x00000000000011a9 <+0>:     endbr64                        # sum_till_MAX的起始地址为0x11a9
    0x00000000000011ad <+4>:     push   %rbp                    # 在栈上保存上层栈帧的地址(存于%rbp)的值
    0x00000000000011ae <+5>:     mov    %rsp,%rbp               # 更新当前栈帧地址(栈顶地址%rsp)到栈指针%rbp
    0x00000000000011b1 <+8>:     sub    $0x20,%rsp              # 在栈上分配用于保存自动变量的空间(栈按照地址减小的方向增长)
    0x00000000000011b5 <+12>:    mov    %edi,-0x14(%rbp)        # 开始执行sum_till_MAX
    0x00000000000011b8 <+15>:    addl   $0x1,-0x14(%rbp)        # n++
    0x00000000000011bc <+19>:    mov    -0x14(%rbp),%eax
    0x00000000000011bf <+22>:    mov    %rax,-0x8(%rbp)         # sum = n, M[%rbp0-0x8]存了sum的值
    0x00000000000011c3 <+26>:    mov    0x2e47(%rip),%eax       # 0x4010 <max_addend>
    0x00000000000011c9 <+32>:    cmp    %eax,-0x14(%rbp)
    0x00000000000011cc <+35>:    jae    0x11dc <sum_till_MAX+51># 结束递归
    0x00000000000011ce <+37>:    mov    -0x14(%rbp),%eax
    0x00000000000011d1 <+40>:    mov    %eax,%edi
    0x00000000000011d3 <+42>:    callq  0x11a9 <sum_till_MAX>   # 递归调用sum_till_MAX
    0x00000000000011d8 <+47>:    add    %rax,-0x8(%rbp)
    0x00000000000011dc <+51>:    mov    -0x8(%rbp),%rax         # 将sum的值通过%rax返回
    0x00000000000011e0 <+55>:    leaveq                         # 释放当前栈，更新%rbp的值到上一栈帧
    0x00000000000011e1 <+56>:    retq                           # 跳转到栈中的返回地址
    ```

### GDB的backtrace


### 栈大小的限制


