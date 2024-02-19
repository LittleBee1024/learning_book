# 内存调试工具

> [《高效C/C++调试》 - 严琦](http://www.tup.tsinghua.edu.cn/Wap/tsxqy.aspx?id=10204101)，第十章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/10/code)中找到

内存调试工具大致分三类：

* 填充字节方法
    * 在每个内存块的开头或末尾添加额外的**填充字节**，如果这些填充字节发生改变，说明存在非法的内存修改
* 系统保护页方法
    * 在可能被越界的内存块前后设置一个不可访问的系统保护页，如果程序试图访问受保护的内存，系统就可以通过硬件检测到这种操作
* 动态二进制分析
    * 常见的内存检测工具`Valgrind`和`Google Address Sanitizer`都是通过此方法实现的
    * 采用**影子内存**来跟踪程序的实时内存使用

## ptmalloc's MALLOC_CHECK_

`glibc`内存管理器`ptmalloc`实现了一个基于填充的简单且非常有用的调试特性。我们可以通过将环境变量`MALLOC_CHECK_`设置为非零数值来启用它。该值的最低两位决定了`ptmalloc`在检测到错误时应采取的操作：

* 如果最低位设置了(例如1)，那么每次检测到错误时都会在stderr上打印错误消息
* 如果第二位设置了(例如2)，那么只要核心转储打开，`ptmalloc`就会在检测到错误时终止进程并生成一个核心转储文件


[例子“mem_check”](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/10/code/mem_check)展示了`MALLOC_CHECK_`环境变量的作用。

```cpp
int main()
{
   int *array = new int[100];
   array[100] = 0; // error
   delete[] array;
   return 0;
}
```

上面的代码存在越界访问的问题，正常执行程序并不会报任何错误，但通过设置`MALLOC_CHECK_`环境变量，可检测出程序存在错误：

```bash
sh> ./main
sh> env MALLOC_CHECK_=1 ./main
*** Error in `./main': free(): invalid pointer: 0x000000000149cc20 ***
# 由于没有开启核心存储，所以此处只是因内存非法访问而中断了程序
sh> env MALLOC_CHECK_=2 ./main
Abort
```
