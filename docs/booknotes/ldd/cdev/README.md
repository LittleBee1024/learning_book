# 字符设备驱动

> [《Linux设备驱动程序》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80d6mS7pO12K6Qb30)的第3章，[《Linux设备驱动开发详解》 - 宋宝华 ](https://1drv.ms/b/s!AkcJSyT7tq80eFABEg8fSOajqHk)的第6章，的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/cdev/code)中找到

## 设备号
* 主设备号
    * 标识设备对应的驱动程序
* 次设备号
    * 由内核使用，用于确定设备文件(通常位于`/dev`目录)所指的设备

在内核中，主/次设备号通过`MKDEV(int major, int minor)`生成`dev_t`类型的设备编号，并通过下面的函数完成分配/释放设备编号的工作：

```cpp
#include <linux/fs.h>

// 静态分配设备编号，用于已知起始设备号的情况
//  first - 要分配的设备编号范围的起始值，常设置为0
//  count - 所请求的连续设备编号的个数，此数值会影响可用次设备编号的个数
//  name - 是和该编号范围关联的设备名称，将出现在`/proc/devices`和`sysfs`中
int register_chrdev_region(dev_t from, unsigned count, const char *name);

// 动态分配设备编号，用于设备号未知，向系统动态申请未被占用的设备号的情况
//  dev - 输出参数，保存调用完成后分配的第一个编号
//  baseminor - 应该要使用的被请求的第一个次设备号，通常是0
//  count和name - 和上面的函数一致
int alloc_chrdev_region(dev_t * dev, unsigned baseminor, unsigned count, const char *name);

// 释放设备编号
void unregister_chrdev_region (dev_t from, unsigned count);
```
[模块"dev_num"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/cdev/code/dev_num)在加载时向系统申请了一个主设备号`111`，在卸载时释放了此设备号：

```cpp title="mock.c"
#define MOCK_MAJOR 111

static int mock_init(void)
{
   int ret;

   printk(KERN_INFO "Mock enter\n");

   ret = register_chrdev_region(MKDEV(MOCK_MAJOR, 0), 1, "mock");
   if (ret < 0)
   {
      printk(KERN_ERR "Failed to register major number %d for mock module\n", MOCK_MAJOR);
      return ret;
   }

   printk(KERN_INFO "Register major number %d for mock module", MOCK_MAJOR);
   return 0;
}
module_init(mock_init);

static void mock_exit(void)
{
   printk(KERN_INFO "Mock exit\n");
   unregister_chrdev_region(MKDEV(MOCK_MAJOR, 0), 1);
}
module_exit(mock_exit);
```

```bash
> sudo insmod mock.ko
# 申请的设备会出现在`/proc/devices`中
> cat /proc/devices | grep mock
111 mock

> sudo rmmod mock.ko
> tail -n 3 /var/log/kern.log
Jun 19 21:22:18 ben-vm-base kernel: [213898.642963] Mock enter
Jun 19 21:23:05 ben-vm-base kernel: [213898.642967] Register major number 111 for mock module
Jun 19 21:23:05 ben-vm-base kernel: [213944.901289] Mock exit
```
