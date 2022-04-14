# 动态链接

> [《程序员的自我修养--链接、装载与库》 - 俞甲子，石凡，潘爱民](https://1drv.ms/b/s!AkcJSyT7tq80cUuHb2eRcJkkBjM?e=YUwBqB)，第七章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cxydzwxy/compile_link/dynamic_link/code)中找到

## 基本实现
在Linux系统中，ELF动态链接文件被称为动态共享对象(DSO, Dynamic Shared Objects)，简称**共享对象**，它们一般都是以".so"为扩展名的一些文件。

程序和动态链接库之间的链接工作是由动态链接器完成的。不同于静态链接器，动态链接器把链接过程从程序装载前推迟到了装载的时候。这种方法称为延迟绑定(Lazy Binding)，虽然损失了一些运行时的性能，但是提高了灵活性。

### 简单的例子




