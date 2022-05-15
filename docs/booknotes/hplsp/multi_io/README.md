# IO复用

> [《Linux高性能服务器编程》 - 游双 ](https://1drv.ms/b/s!AkcJSyT7tq80c1DmkdcxK7oScvQ)的第9章，[《UNIX网络编程卷1》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80dP1Vghbg7qb9uts)的第6章，以及[《小林coding - 网络系统》](https://xiaolincoding.com/os)的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/multi_io/code)中找到

## IO复用函数

![io_multiplex](./images/io_multiplex.png)

Linux提供了`select()`，`poll()`，`epoll()`三个函数，用于监控文件描述符上发生的事件，实现IO复用。其中，`select()`，`poll()`是类UNIX系统都提供的，而`epoll()`是Linux独有的。

下面通过三个例子，分别介绍这三个函数的使用方法。这三个例子可同时处理新的客户端网络连接和多个客户端输入，并将客户端传来的字符串转换成大写后回传给客户端：

![io_multiplex_examples](./images/io_multiplex_examples.png)

### select

### poll

### epoll

