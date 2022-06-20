# 字符设备驱动

> [《Linux设备驱动程序》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80d6mS7pO12K6Qb30)的第3章，[《Linux设备驱动开发详解》 - 宋宝华 ](https://1drv.ms/b/s!AkcJSyT7tq80eFABEg8fSOajqHk)的第6章，的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/cdev/code)中找到

![cdev_flow](./images/cdev_flow.png)

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

## 字符设备驱动组成

字符设备驱动主要有两部分组成：

* 模块加载/卸载函数
    * 在加载函数中应实现设备号的申请和`cdev`的注册
    * 在卸载函数中应实现设备号的释放和`cdev`的注销
* `file_operations`结构体中的成员函数
    * 大多数字符设备驱动会实现`read()`，`write()`和`ioctl()`函数

### `cdev`结构体

```cpp
#include <linux/cdev.h>

struct cdev {
    struct kobject kobj;
    struct module *owner;
    const struct file_operations *ops;  // 文件操作结构体
    struct list_head list;
    dev_t dev;                          // 设备号
    unsigned int count;
};

// 向内核申请`cdev`空间
struct cdev *cdev_alloc(void);

// 初始化`cdev`的成员，并建立`cdev`和`file_operations`之间的连接
void cdev_init(struct cdev * cdev, const struct file_operations * fops);

// 向系统添加一个`cdev`设备
//  num - 设备编号，由主/次设备号组成
//  count - 和该设备关联的设备编号的数量，常取值1
int cdev_add(struct cdev *dev, dev_t num, unsigned int count);

// 从系统删除一个`cdev`设备
void cdev_del(struct cdev *);
```

### `file_operations`结构体
向系统申请了设备编号后，需要将驱动程序操作连接到这些编号上，`file_operations`结构就是用来建立这种连接的。

```cpp
#include <linux/fs.h>

struct file_operations {
    // 用于修改文件的当前读写位置
    loff_t (*llseek) (struct file *, loff_t, int);
    // 从设备中读取数据
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    // 向设备发送数据
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *)；
    // 提供设备相关控制命令的实现
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    // 将设备内存映射到进程的虚拟地址空间中
    int (*mmap) (struct file *, struct vm_area_struct *);
    // 设备文件执行的第一个操作，提供了给驱动程序初始化的能力
    int (*open) (struct inode *, struct file *);
    // file结构被释放时，将调用这个操作
    //  不是每次调用close时都会被调用，只要file结构被空闲(如fork或dup调用之后)，release就会等到
    //  所有副本都关闭之后才会调用
    int (*release) (struct inode *, struct file *);
    ...
};

// 系统中每个打开的文在内核空间中都有一个对应的`file`结构
struct file {
    struct inode *f_inode;
    // 文件模式
    fmode_t f_mode;
    // 文件读写位置
    loff_t f_pos;
    ...
};

// 对于单个文件可能会有多个对应的`file`结构体，但只有一个`inode`结构
struct inode {
    // 设备编号
    dev_t i_rdev;
    union {
        // 字符设备内部结构
        struct cdev *i_cdev;
        ...
    };
}；
```

### 驱动读/写操作

`read`和`write`方法完成的任务是相似的，即拷贝数据到应用程序空间，或从应用程序空间拷贝数据到内核空间：

```cpp
ssize_t xxx_read(struct file *filep, char __user *buf, size_t count, loff_t *f_pos)
{
    ...
    copy_to_user(..., buf, ...);
    ...
}

ssize_t xxx_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos)
{
    ...
    copy_from_user(..., buf, ...);
    ...
}
```

`read`和`write`方法的`buf`参数是用户空间的指针，不能直接在内核中直接引用，原因是：

* 内核模式中用户空间的指针可能是无效的
* 用户空间的内存是分页的，访问时有可能发生页错误，而内核代码是不允许发生页错误的
* 保护内核内存，防止用户操作破坏内核

因此，可通过下面的函数完成内核空间核用户空间的数据传输：
```cpp
// 连续空间
unsigned long copy_from_user(void *to, const void __user *from, unsigned long count);
unsigned long copy_to_user(void __user *to, const void *from, unsigned long count);

// 简单类型，如：char, int, long等
int val;                    // 内核空间整型变量
get_user(val, (int *) arg); // 用户→内核，arg是用户空间的地址
put_user(val, (int *) arg); // 内核→用户，arg是用户空间的地址
```

### 驱动I/O控制操作
