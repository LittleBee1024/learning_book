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



