# 系统级I/O

> [《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG) 第十章，以及[《Linux/UNIX 系统编程手册》 - Michael Kerrisk](https://1drv.ms/b/s!AkcJSyT7tq80fmGEtgQjbyZPMOY?e=Z7XvIW) 文件系统相关章节（4，5，13~18）的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/10/code)中找到

## 虚拟文件系统

在Linux系统中基本上把所有内容都看作文件，除了我们普通意义理解的文件之外，目录、字符设备、块设备、FIFO或管道、套接字、符号链接等都被视为是一个“文件”。

![vfs](./images/vfs.png)

如上图所示，虚拟文件系统(VFS)VFS是一个抽象层，其向上提供了统一的文件访问接口，而向下则兼容了各种不同的文件系统。不仅仅是诸如Ext2、Ext4、XFS和Btrfs等常规意义上的文件系统，还包括伪文件系统和设备等等。另外，VFS实现了一部分公共的功能，例如：页缓存和inode缓存等，从而避免多个文件系统重复实现的问题。

Linux上所有文件系统中的文件都位于单根目录树下，树根就是根目录`/`。其他的文件系统都挂载在根目录下。`mount device directory`命令会将名为`device`的文件系统挂接到目录层级中由`directory`所指定的目录，即文件系统的挂载点。`mount`命令可以列出当前已挂载的文件系统，例如：
```bash
> mount
proc on /proc type proc (rw)
sysfs on /sys type sysfs (rw)
tmpfs on /run type tmpfs (rw)
udev on /dev type devtmpfs (rw)
/dev/sda6 on / type ext4 (rw)
/dev/sda8 on /home type ext3 (rw,acl,user_xattr)
/dev/sda1 on /windows/C type vfat (rw,noexec,nosuid,nodev)
/dev/sda9 on /home/mtk/test type reiserfs (rw)
```

![mount](./images/mount.png)

除了上图中驻留在磁盘上的文件系统，Linux同样支持驻留于内存中的虚拟文件系统：proc，sysfs，tmpfs，devtmpfs等。`/proc/filesystems`可以查看当前为内核所知的文件系统类型。

## 常规文件系统

常规文件系统是对常规文件和目录的组织集合，由`mkfs`命令创建，其类型有：

* 传统的ext2文件系统
* 各种原始UNIX文件系统，如，Minix、System V以及BSD文件系统
* 微软的FAT、FAT32以及NTFS文件系统
* ISO 9660 CD-ROM文件系统
* Apple Macintosh的HFS
* 一系列网络文件系统，如，SUN的NFS、IBM和微软的SMB、Novell的NCP等
* 一系列日志文件系统，包括ext3、ext4、Reiserfs、JFS、XFS以及Btrfs等

### 磁盘和分区

常规文件和目录通常都存放在磁盘设备里(比如，CD-ROM、flash内存卡以及虚拟磁盘等)。每块磁盘被划分为一个或多个分区。内核将每个分区视为位于`/dev`路径下的单独设备。磁盘分区一般存放三种信息：

* 文件系统
    * 用来存放常规文件
* 数据区域
    * 可做为裸设备对其进行访问，如数据库管理系统就使用了该技术
* 交换区域
    * 供内核的内存管理使用，如Linux系统中的`/proc/swaps`可查看已激活的交换区域信息

