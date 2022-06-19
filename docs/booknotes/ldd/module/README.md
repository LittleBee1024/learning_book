# 内核模块

> [《Linux设备驱动程序》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80d6mS7pO12K6Qb30)的第2章，[《Linux设备驱动开发详解》 - 宋宝华 ](https://1drv.ms/b/s!AkcJSyT7tq80eFABEg8fSOajqHk)的第4章，的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/module/code)中找到

## 模块初探

[模块"Hello World"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/module/code/hello)实现了一个最简单的模块，其中["hello.c"](./code/hello/hello.c)定义了两个函数：

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

`insmod`和`modprobe`可用于装载模块。`modprobe`命令更强大，会同时加载该模块所依赖的其他模块，因此需要在`/lib/modules/<kernel-version>/modules.dep`文件中指定依赖关系。

模块装载完成后，会在下面两个目录中加入相关信息：

* /proc/modules
    * `lsmod`命令就是读取`/proc/modules`文件中的信息
* /sys/module/<module-name>

```bash
# 装载模块
> sudo insmod hello.ko
> tail -n 1 /var/log/kern.log
Jun 19 12:09:34 ben-vm-base kernel: [181105.450980] Hello World enter

# 查看模块
> cat /proc/modules | grep hello
hello 16384 0 - Live 0x0000000000000000 (OE)
> lsmod | grep hello
hello                  16384  0
> ls /sys/module/hello/
coresize  holders  initsize  initstate  notes  refcnt  sections  srcversion  taint  uevent

# 卸载模块
> sudo rmmod hello.ko
> tail -n 2 /var/log/kern.log
Jun 19 12:09:34 ben-vm-base kernel: [181105.450980] Hello World enter
Jun 19 12:10:23 ben-vm-base kernel: [181154.345969] Hello World exit
```


## 模块参数

[模块"book"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/module/code/book)通过`module_param`添加了模块参数，因此在装载时，利用`book_name=<name> book_num=<num>`即可自定义模块的参数内容，`/sys/module/book/parameters`中记录了参数的内容。

```cpp title="book.c"
static char *book_name = "Linux Device Driver";
module_param(book_name, charp, S_IRUGO);

static int book_num = 4000;
module_param(book_num, int , S_IRUGO);
```
```bash title="装载模块时传入参数"
> sudo insmod book.ko book_name="Good_Book" book_num=1000

> cat /sys/module/book/parameters/book_name
Good_Book
> cat /sys/module/book/parameters/book_num
1000
```

## 模块符号

`insmod`使用公共内核符号表来解析模块中未定义的符号。内核符号表可通过`cat /proc/kallsyms`查看。

当模块装入内核后，可通过下面的宏导出的任何符号，导出的符号会变成内核符号表的一部分，供其他模块使用：

* `EXPORT_SYMBOL(name)`
* `EXPORT_SYMBOL_GPL(name)`

[模块"symbol"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/module/code/symbol)中，"export"模块导出了`hello_export`函数，"import"模块使用了`hello_export`函数:

=== "export.c"

    ```cpp
    void hello_export(void)
    {
        printk(KERN_INFO "Hello from another module");
    }
    EXPORT_SYMBOL(hello_export);
    ```

=== "import.c"

    ```cpp
    extern void hello_export(void);

    static int import_init(void)
    {
        hello_export();
        printk(KERN_INFO "Import module enter\n");
        return 0;
    }
    module_init(import_init);
    ```

```bash
# 先装载export, 再装载import
> sudo insmod export.ko
> sudo insmod import.ko
> tail -n 3 /var/log/kern.log
Jun 19 14:39:40 ben-vm-base kernel: [190111.168523] Export module enter
Jun 19 14:39:40 ben-vm-base kernel: [190111.185477] Hello from another module
Jun 19 14:39:40 ben-vm-base kernel: [190111.185481] Import module enter

# import依赖export
> modinfo import.ko
...
depends:        export
...

# 查看公共内核符号表
> cat /proc/kallsyms | grep hello_export
0000000000000000 r __kstrtab_hello_export       [export]
0000000000000000 r __kstrtabns_hello_export     [export]
0000000000000000 r __ksymtab_hello_export       [export]
0000000000000000 T hello_export [export]

# 先卸载import, 再卸载export
> sudo rmmod import.ko
> sudo rmmod export.ko
> tail -n 5 /var/log/kern.log
Jun 19 14:39:40 ben-vm-base kernel: [190111.168523] Export module enter
Jun 19 14:39:40 ben-vm-base kernel: [190111.185477] Hello from another module
Jun 19 14:39:40 ben-vm-base kernel: [190111.185481] Import module enter
Jun 19 14:41:49 ben-vm-base kernel: [190240.103068] Import module exit
Jun 19 14:41:49 ben-vm-base kernel: [190240.138433] Export module exit
```
