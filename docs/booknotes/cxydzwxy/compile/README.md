# 编译

> [《程序员的自我修养--链接、装载与库》 - 俞甲子，石凡，潘爱民](https://1drv.ms/b/s!AkcJSyT7tq80cUuHb2eRcJkkBjM?e=YUwBqB)，第二、三章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/compile/code)中找到

## 生成一个可执行文件
C语言生成一个可执行文件需要4个步骤：

* 预处理(Preprocessing)
* 编译(Compilation)
* 汇编(Assembly)
* 链接(Linking)

下面以[HelloWorld](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/compile/code/hello)为例，分别介绍各步骤做了什么事情。

### 预编译
预编译过程通过`gcc -E main.c -o main.i`命令，处理源代码中以`#`开头的预编译指令。主要规则如下：

* 展开宏和头文件
* 删除注释
* 添加行号和文件名标识，以便编译器产生调试用的行号信息，例如`# 2 "main.c" 2`，表示main.c的第二行
    * 文件名后面的数字代表不同的意思，参考[GCC手册](https://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html)
        * `1`，表示新文件的开始
        * `2`，表示返回此文件
        * `3`，表示接下来的内容来自系统头文件，某些警告将被忽略
        * `4`，表示接下来的内容按C代码处理，即被`extern "C"`修饰

### 编译
编译过程就是把预处理完的文件进行一系列词法分析、语法分析、语义分析以及优化后产生相应的汇编代码文件，其命令是：`gcc -S main.i -o main.s`。

### 汇编
汇编器将汇编代码转变成机器可执行的指令，称为目标文件(Object File)。我们可以调用汇编器`as`完成，也可以通过GCC命令完成。

* `as main.s -o main.o`
* `gcc -c main.s -o main.o`

### 链接
GCC命令`gcc main.o -o main`，可根据目标文件，生成可执行文件。此命令底层是依赖链接器`ld`完成的，其完整命令如下：
```sh
ld -static /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/gcc/x86_64-linux-gnu/9/crtbeginT.o \
	-L/usr/lib/gcc/x86_64-linux-gnu/9 -L/usr/lib -L/lib main.o --start-group -lgcc -lgcc_eh -lc --end-group /usr/lib/gcc/x86_64-linux-gnu/9/crtend.o /usr/lib/x86_64-linux-gnu/crtn.o \
	-o main
```

