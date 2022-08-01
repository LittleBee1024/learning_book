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

