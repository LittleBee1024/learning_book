# 处理器体系结构

> [《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG)，第四章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/04/code)中找到

一个处理器支持的指令和指令的字节级编码称为它的**指令集体系结构**(Instruction-Set Architecture, ISA)。ISA在编译器编写者和处理器设计人员之间提供了一个概念抽象层，编译器编写者只需要知道允许哪些指令，以及它们是如何编码的；而处理器设计者必须建造出执行这些指令的处理器。

本章将简要介绍处理器硬件的设计：

* 首先定义一个简单的指令集“Y86-64”
* 然后介绍一种硬件控制语言(Hardware Control Language, HCL)，用于描述我们的处理器设计
    * 先设计一个基于**顺序**操作、功能正确的Y86-64处理器
    * 再创建一个**流水线**化的处理器
* 最后还会设计一些工具来研究和测试处理器设计
    * 包括Y86-64的汇编器、Y86-64的模拟器(基于HCL文件)

## Y86-64指令集体系结构

### 程序员可见的状态

![y86_64_visible_state](./images/y86_64_visible_state.png)

Y86-64的程序可以访问和修改程序寄存器、条件码、程序计数器和内存。这些状态相当于Y86-64的硬件接口，在接下来分析Y86_64指令的过程中，请时刻关注这些状态的变化。

### Y86-64指令

![y86_64_inst](./images/y86_64_inst.png)

上图显示了Y86-64各指令的结构。虽然不同指令的长度和功能都不同，但是为了最大限度地复用硬件电路，每个指令都遵循相同的执行模式，详情可参考["将处理组织成阶段"](#_5)章节

### Y86-64程序

根据Y86-64指令规则，下面给出`sum`函数在Y86-64下的汇编代码：

=== "C代码"

    ```cpp
    long sum(long *start, long count)
    {
        long sum = 0;
        while (count)
        {
            sum += *start;
            start++;
            count--;
        }
        return sum;
    }
    ```

=== ""Y86-64"汇编代码"

    ```asm linenums="1"
    # long sum(long *start, long count)
    # start in %rdi, count in %rsi
    sum:
        irmovq $8,%r8       # 将常数8放入寄存器
        irmovq $1,%r9       # 将常数1放入寄存器
        xorq %rax,%rax      # sum = 0
        andq %rsi,%rsi      # 初始化条件码，非零标志
    loop:
        mrmovq (%rdi),%r10  # 获取*start值
        addq %r10,%rax      # sum += *start
        addq %r8,%rdi       # start++
        subq %r9,%rsi       # count--，并设置条件码
    test:
        jne loop            # 当count非零时，跳转
        ret
    ```

=== ""x86-64"汇编代码"

    ```asm linenums="1"
    # long sum(long *start, long count)
    # start in %rdi, count in %rsi
    sum:
        movl $0,%eax        # sum = 0
        jmp .L2             # 跳转到test
    .L3:                    # loop:
        addq (%rdi),%rax    # sum += *start
        addq $8,%rdi        # start++
        subq $1,%rsi        # count--
    test:
        testq %rsi,%rsi
        jne .L3             # 当count非零时，跳转
        ret
    ```

## 逻辑设计和硬件控制语言HCL

要实现一个数字系统需要三个主要的组成部分：

* 计算对位进行操作的函数的组合逻辑
* 存储位的存储单元
* 控制存储器单元更新的时钟信号

### 组合电路

将很多的逻辑门组合成一个网，就能构建计算块(computational block)，称为组合电路(combinational circuits)。如何构建这些网有几个限制：

* 每个逻辑门的输入必须是下述选项之一：
    * 一个系统输入
    * 某个存储单元的输出
    * 某个逻辑门的输出
* 两个或多个逻辑门的输出不能连接在一起
* 这个网必须是无环的，即网中不能有路径经过一系列的门而形成一个回路

### 时序电路

组合电路不存储任何信息。为了产生**时序电路**(sequential circuit)，也就是有状态并且在这个状态上进行计算的系统，我们必须引入按位存储信息的设备，包括：程序计数器、条件码、寄存器文件、内存等。时序电路是作为电路不同部分中的组合逻辑之间的屏障。

下图给出了程序计数器、状态码、寄存器文件和内存4个状态单元。组合逻辑(例如ALU)环绕着条件码寄存器，产生输入到条件码寄存器。而其他部分(例如分支计算和PC选择逻辑)又将条件码寄存器作为输入。

![seq_circuit](./images/seq_circuit.png)

上图中蓝色部分表示`addq`指令相关单元，

* 在时钟周期3开始的时候(点1)
    * 开始`addq`指令的组合逻辑工作
    * 状态单元更新为`irmovq`指令的状态
* 在时钟周期3末尾的时候(点2)
    * **`addq`指令组合逻辑工作完成，为其状态单元准备好了新值**
    * 此时状态单元还维持着`irmovq`指令的状态
* 在时钟周期4开始的时候(点3)
    * 开始`je`指令的组合逻辑工作
    * **状态单元更新为`addq`指令的状态**
* 在时钟周期4末尾的时候(点4)
    * `je`指令组合逻辑工作完成，为其状态单元准备好了新值
    * **此时状态单元还维持着`addq`指令的状态**

## Y86-64的顺序实现

### 将处理组织成阶段

为了统一，将Y86-64指令的执行过程分为五个阶段：

* 取值(fetch)
    * 从内存中取出不同大小的指令，获取`rA`，`rB`，`valC`，并计算`valP`
* 译码(decode)
    * 获取`valA`和`valB`
* 执行(execute)
    * 计算`valE`和状态码`CC`
* 访存(memory)
    * 读写内存，读出的内存值存于`valM`
* 写回(write back)
    * 将`valE`或`valM`写入对应的寄存器
* 更新PC(PC update)
    * 除了条件跳转指令和`ret`外，都能在取指阶段确定下一条指令的地址
        * `call`和`jmp`(无条件转移)，PC的更新值为`valC`
        * 其他指令，PC的更新值为`valP`

下面介绍不同类型的指令在各阶段的动作：

* 传送指令mov - 向寄存器或内存传递数据，不涉及条件码

    | 阶段 | rrmovq rA,rB | irmovq V,rB | rmmovq rA,D(rB) | mrmovq D(rB),rA |
    | --- | --- | --- | --- | --- |
    | 取指 | icode:ifun <- M1[PC]</br>rA:rB <- M1[PC+1]</br><br>valP <- PC+2 | icode:ifun <- M1[PC]</br>rA:rB <- M1[PC+1]</br>valC <- M8[PC+2]<br>valP <- PC+10 | icode:ifun <- M1[PC]</br>rA:rB <- M1[PC+1]</br>valC <- M8[PC+2]<br>valP <- PC+10 | icode:ifun <- M1[PC]</br>rA:rB <- M1[PC+1]</br>valC <- M8[PC+2]<br>valP <- PC+10 |
    | 译码 | valA <- R[rA]</br> | | valA <- R[rA]</br>valB <- R[rB] | </br>valB <- R[rB] |
    | 执行 | valE <- 0+valA | valE <- 0+valC | valE <- valB+valC | valE <- valB+valC |
    | 访存 | | | M8[valE] <- valA | valM <- M8[valE] |
    | 写回 | R[rB] <- valE | R[rB] <- valE | | R[rA] <- valM |
    | 更新PC | PC <- valP | PC <- valP | PC <- valP | PC <- valP |

* 算术运算指令OP - 对两个寄存器值进行算术运算，并修改条件码，不涉及内存访问

    | 阶段 | OPq rA, rB |
    | --- | --- |
    | 取指 | icode:ifun <- M1[PC]</br>rA:rB <- M1[PC+1]</br>valP <- PC+2 |
    | 译码 | valA <- R[rA] </br> valB <- R[rB] |
    | 执行 | valE <- valB OP valA</br>Set CC |
    | 访存 | |
    | 写回 | R[rB] <- valE |
    | 更新PC | PC <- valP |

* 栈操作指令 - 既要访问内存，又要访问寄存器，不涉及条件码

    | 阶段 | pushq rA | popq rA |
    | --- | --- | --- |
    | 取指 | icode:ifun <- M1[PC]</br>rA:rB <- M1[PC+1]</br>valP <- PC+2 | icode:ifun <- M1[PC]</br>rA:rB <- M1[PC+1]</br>valP <- PC+2 |
    | 译码 | valA <- R[rA]</br>valB <- R[%rsp] | valA <- R[%rsp]</br>valB <- R[%rsp] |
    | 执行 | valE <- valB+(-8) | valE <- valB+8 |
    | 访存 | M8[valE] <- valA | valM <- M8[valA] |
    | 写回 | R[%rsp] <- valE | R[%rsp] <- valE</br>R[rA] <- valM |
    | 更新PC | PC <- valP | PC <- valP |

* 跳转指令 - 对程序计数器的处理方式与其他指令不同，其他指令PC更新值都是`valP`

    | 阶段 | jXX Dest | call Dest | ret |
    | --- | --- | --- | --- |
    | 取指 | icode:ifun <- M1[PC]</br>valC <- M8[PC+1]</br>valP <- PC+9 |  icode:ifun <- M1[PC]</br>valC <- M8[PC+1]</br>valP <- PC+9 | icode:ifun <- M1[PC]</br></br>valP <- PC+1 |
    | 译码 | | </br>valB <- R[%rsp] | valA <- R[%rsp]</br>valB <- R[%rsp] |
    | 执行 | </br>Cnd <- Cond(CC, ifun) | valE <- valB+(-8) | valE <- valB+8 |
    | 访存 | | M8[valE] <- valP | valM <- M8[valA] |
    | 写回 | | R[%rsp] <- valE | R[%rsp] <- valE |
    | 更新PC | PC <- Cnd?valC:valP | PC <- valC | PC <- valM |

## Y86-64的流水线实现

### 流水线的通用原理
