# 虚拟内存

> [《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG)，第九章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/09/code)中找到

## 物理和虚拟寻址

![MMU](./images/MMU.png)

上图显式了，现代处理器使用**虚拟地址(virtual addressing)**的寻址形式。CPU芯片上的内存管理单元(Memory Management Unit, MMU)利用存放在主存中的查询表来动态翻译虚拟地址，该表的内容由操作系统管理。

## 虚拟内存作为缓存的工具

VM系统通过将虚拟内存分割为称为**虚拟页(Virtual Page, VP)**的大小固定的块来处理这个问题。类似地，物理内存被分割为相同大小的**物理页(Physical Pge, PP)**。任意时刻，虚拟页面的集合都分为三个不相交的子集：

* 未分配的
* 缓存的
* 未缓存的

虚拟内存系统必须有某种方法来判定一个**已分配的虚拟页**是否缓存在DRAM中的某个地方：

* 如果已在物理页中缓存，系统还必须确定这个虚拟页存放在哪个物理页中
* 如果未在物理页中缓存，系统还必须判断这个虚拟页存放在**磁盘**的哪个位置，在物理内存中选择一个牺牲页，并将虚拟页从磁盘复制到DRAM中，以替换这个牺牲页

### 页命中

![vm_page_hit](./images/vm_page_hit.png)

### 缺页

![vm_page_fault](./images/vm_page_fault.png)
