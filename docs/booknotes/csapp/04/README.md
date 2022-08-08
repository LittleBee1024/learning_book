# 处理器体系结构

> [《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG)，第四章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/04/code)中找到

一个处理器支持的指令和指令的字节级编码称为它的**指令集体系结构**(Instruction-Set Architecture, ISA)。ISA在编译器编写者和处理器设计人员之间提供了一个概念抽象层，编译器编写者只需要知道允许哪些指令，以及它们是如何编码的；而处理器设计者必须建造出执行这些指令的处理器。

本章将简要介绍处理器硬件的设计。首先定义一个简单的指令集“Y86-64”。接下来介绍一种硬件控制语言(Hardware Control Language, HCL)用于描述我们的处理器设计。先设计一个基于**顺序**操作、功能正确的Y86-64处理器。再创建一个**流水线**化的处理器。最后还会设计一些工具来研究和测试处理器设计。包括Y86-64的汇编器、Y86-64的模拟器(基于HCL文件)。
