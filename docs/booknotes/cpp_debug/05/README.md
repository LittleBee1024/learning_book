# 优化后的二进制

> [《高效C/C++调试》 - 严琦](http://www.tup.tsinghua.edu.cn/Wap/tsxqy.aspx?id=10204101)，第五章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/05/code)中找到


## 调试版和发行版的区别

调试版和发行版的具体区别如下：

* 内存分配算法不同
    * Windows调试版可能分配比发行版更多的字节，从而掩盖了用户内存溢出的问题
* 由于条件编译导致执行语句的不同
    * 例如，`assert`语句就只在调试版中生效
* 局部变量和函数参数的存储位置可能会有所不同
    * 优化器会尽可能地利用寄存器，而调试版总会为局部变量和参数分配栈内存
    * 例如，对于未初始化变量仅发行版中存在问题，原因可能是，在调试版中该变量存在栈上，而发行版中存在寄存器中。栈上未初始化的内存可能是一个可以容忍的数值，而寄存器中原先的值更加随机，因而更容易发现问题
* 优化器会优化代码执行
    * 对于比较激进的优化器，有时可能会因改变了代码执行顺序，而产生不期望的结果

## 调试优化代码的挑战

调试优化程序最具挑战性的部分可能是**检查局部变量和函数参数的值**。根据函数的复杂程度、优化级别的选择和编译器的特定实现，调试器可能在某些上下文中拒绝显示某些局部变量的值或打印处完全错误的值。它与优化器放置局部变量的位置以及它们在程序运行时如何更新有很大关系：

* 优化后的程序不一定每次都更新局部变量在内存中的值(有可能在寄存器间操作)，导致内存位置不反应当前位置的值
* 由于执行顺序的优化，导致局部变量的修改时间可能和源代码不一致
* 没有任何副作用的局部变量可能被优化器删除

与局部变量和函数参数不同，全局变量不会受到优化的任何影响，因为全局变量具有固定的内存位置。

无论是发行版还是调试版，都建议总是打开生成调试符号的编译选项，即`-g`编译选项。

## 汇编代码介绍

下表总结了x86-64寄存器是如何用于参数传递和返回值的。

| 寄存器 | 用法 | 由被调用者保存 |
| ---- | --- | --- |
| rax | 临时寄存器，第1个返回值，可变参数时SSE寄存器的个数 | No |
| rbx | 基地址 | Yes |
| rcx | 第4个整数参数 | No |
| rdx | 第3个整数参数，第2个返回值 | No |
| rsp | 栈指针（指向栈顶，最低地址） | Yes |
| rbp | 栈帧指针（指向当前函数栈帧开始的地方） | Yes |
| rsi | 第2个整数参数 | No |
| rdi | 第1个整数参数 | No |
| r8  | 第5个整数参数 | No |
| r9  | 第6个整数参数 | No |
| r10 | 传递函数静态链接参数 | No |
| r11 | 临时寄存器 | No |
| r12 ~ r15 | 临时寄存器 | Yes |
| xmm0 ~ xmm7 | 传递浮点数参数 | No |
| xmm8 ~ xmm15 | 临时寄存器 | No |
| mmx0 ~ mxx7 | 临时寄存器 | No |
| st0, st1 | 临时寄存器，返回`long double`类型 | No |
| st2 ~ st7 | 临时寄存器 | No |
| fs | 线程特有寄存器 | No |

### 汇编程序的结构

所有函数的开头和结尾都非常相似，它们分别被称为函数的序言（Prologue）和结语（Epilog）。函数序验给被调用函数设置一个栈帧，而结语做相反的事情，释放栈帧并返回到前一个调用函数的栈帧。典型的序言的片段如下：

```asm
pushq %rbp          # 将当前函数的栈帧指针入栈保存
movq %rsp, %rbp     # 将栈帧指针跟新为栈顶位置，为被调用函数栈开始的地方
subq $56, %rsp      # 将栈指针下移56字节，为被调用函数分配了56字节的栈空间
```

以下面的代码["prologue"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/05/code/prologue)为例：

```cpp
#include <stdio.h>

int f(int num)
{
   return num;
}

int main(int argc, char **argv)
{
   f(5);
}
```

其汇编代码如下：

```cpp
0000000000000000 <f>:
#include <stdio.h>

int f(int num)
{
   0:	f3 0f 1e fa          	endbr64 
   4:	55                   	push   %rbp
   5:	48 89 e5             	mov    %rsp,%rbp
   8:	89 7d fc             	mov    %edi,-0x4(%rbp)
   return num;
   b:	8b 45 fc             	mov    -0x4(%rbp),%eax
}
   e:	5d                   	pop    %rbp
   f:	c3                   	ret    

0000000000000010 <main>:

int main(int argc, char **argv)
{
  10:	f3 0f 1e fa          	endbr64 
  14:	55                   	push   %rbp
  15:	48 89 e5             	mov    %rsp,%rbp
  18:	48 83 ec 10          	sub    $0x10,%rsp
  1c:	89 7d fc             	mov    %edi,-0x4(%rbp)
  1f:	48 89 75 f0          	mov    %rsi,-0x10(%rbp)
   f(5);
  23:	bf 05 00 00 00       	mov    $0x5,%edi
  28:	e8 00 00 00 00       	call   2d <main+0x1d>
  2d:	b8 00 00 00 00       	mov    $0x0,%eax
}
  32:	c9                   	leave  
  33:	c3                   	ret    
```

`main`函数在调用`f(5)`之前，做了如下的事情：

```asm
push   %rbp                 # 入栈保存栈帧指针
mov    %rsp,%rbp            # 为f()函数设置栈帧栈帧
sub    $0x10,%rsp           # 为f()函数分配16字节栈空间
mov    %edi,-0x4(%rbp)      # 入栈保存edi寄存器，为传递第1个参数
mov    %rsi,-0x10(%rbp)     # 入栈保存rsi寄存器，为传递第2个参数
mov    $0x5,%edi            # 第一个参数的值为5，通过edi寄存器传递
call   2d <main+0x1d>       # 调用f()函数
```

`f`函数在返回之前，做了如下的事情：

```asm
mov    -0x4(%rbp),%eax      # 将返回值写入eax寄存器
pop    %rbp                 # 恢复rbp寄存器为main函数的栈帧指针，此栈帧是在函数序言中入栈的
ret                         # 从f函数返回到main函数
```

### 函数参数传递

下面用一个[例子](http://www.tup.tsinghua.edu.cn/Wap/tsxqy.aspx?id=10204101)，第五章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/05/code/para_pass)，介绍一下不同类型的函数参数的传递过程。

```cpp
class POD_STRUCT
{
public:
   short s;
   int a;
   double d;
};

class NONE_POD_STRUCT
{
   virtual bool Verify() { return true; }

public:
   short s;
   int a;
   double d;
};

double Sum(int i_int0,
           int i_int1,
           POD_STRUCT i_pod,
           NONE_POD_STRUCT i_nonpod,
           long *ip_long,
           float i_float,
           long i_long0,
           long i_long1)
{
   double result = i_int0 + i_int1 + i_pod.a + i_pod.d + i_pod.s + i_nonpod.a + i_nonpod.d + i_nonpod.s + *ip_long + i_float + i_long0 + i_long1;

   return result;
}

int main()
{
   int a_int_0 = 0;
   int a_int_1 = 1;
   POD_STRUCT a_pod = {0, 1, 2.2};
   NONE_POD_STRUCT a_nonpod;
   long a_long = 3;
   float a_float = 4.4;

   double sum = Sum(a_int_0, a_int_1, a_pod, a_nonpod, &a_long, a_float, a_long, a_long);

   return 0;
}
```

例子中`sum`函数通过下面的寄存器或栈传递参数：

| 通用寄存器 | 浮点寄存器 | 栈帧 |
| --- | --- | --- |
| %edi: i_int0 | %xmm0: i_pod.d | %rbp+16: i_long1 |
| %esi: i_int1 | %xmm1: i_float | |
| %rdx: i_pod.s, i_pod.a | | |
| %rcx: &i_nonpod | | |
| %r8: ip_long | | |
| %r9: i_long0 | | |

优化器会尽量使用寄存器（而非栈）传递参数，因此上表中只有`i_long1`参数通过栈传递。但是实际编译器会因为优化程度不同，而选择寄存器或者栈传递参数。

## 分析优化后的代码

