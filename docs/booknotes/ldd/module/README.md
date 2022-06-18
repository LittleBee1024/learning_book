# 内核模块

> [《Linux设备驱动程序》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80d6mS7pO12K6Qb30)的第2章，[《Linux设备驱动开发详解》 - 宋宝华 ](https://1drv.ms/b/s!AkcJSyT7tq80eFABEg8fSOajqHk)的第4章，的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/module/code)中找到

## 编译和装载

### Hello World模块
["hello.c"](./code/hello/hello.c)定义了两个函数：

* `hello_init`在模块被装载到内核时被调用
* `hello_exit`在模块被移除时被调用

```cpp
#include <linux/init.h>
#include <linux/module.h>

static int hello_init(void)
{
   printk(KERN_INFO "Hello World enter\n");
   return 0;
}
module_init(hello_init);

static void hello_exit(void)
{
   printk(KERN_INFO "Hello World exit\n ");
}
module_exit(hello_exit);
```





