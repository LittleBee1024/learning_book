# 内核模块

> [《Linux设备驱动程序》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80d6mS7pO12K6Qb30)的第2章，[《Linux设备驱动开发详解》 - 宋宝华 ](https://1drv.ms/b/s!AkcJSyT7tq80eFABEg8fSOajqHk)的第4章，的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/module/code)中找到

## 模块初探

["Hello World"模块](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/module/code/hello)实现了一个最简单的模块，其中["hello.c"](./code/hello/hello.c)定义了两个函数：

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

### 编译模块

编译模块需要准备内核树，可使用Ubuntu系统自带的默认内核目录```/lib/modules/`uname -r`/build```。对于"hello world"模块，

* `obj-m := hello.o`配置指定了模块需要从目标文件`hello.o`中构造
* `make -C $(KDIR) M=$(PWD) modules`命令将当前工作环境和内核构造系统相联系，完成了模块的构建

下面是完整的`Makefile`内容：

```bash
MODULE_NAME := hello

# 如果已经定义KERNELRELEASE，说明当前任务在内核构造系统中，可利用内建语句
ifneq (${KERNELRELEASE},)

obj-m := $(MODULE_NAME).o

else

# 当前系统的内核源代码目录
KDIR ?= /lib/modules/`uname -r`/build

# -C : 改变目录到指定的位置(即内核源代码目录)，其中保存有内核的顶层makefile文件
# M= : 在构造"modules"目标之前返回到模块源代码目录，并指向obj-m变量中设定的模块.o文件
default:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

endif
```

### 装载模块

### 卸载模块


## 模块参数

## 模块符号





