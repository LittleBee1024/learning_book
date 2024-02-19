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

