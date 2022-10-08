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

### 页表

虚拟内存系统必须有某种方法来判定一个**已分配的虚拟页**是否缓存在DRAM中的某个地方：

* 如果已在物理页中缓存，系统还必须确定这个虚拟页存放在哪个物理页中
* 如果未在物理页中缓存，系统还必须判断这个虚拟页存放在**磁盘**的哪个位置，在物理内存中选择一个牺牲页，并将虚拟页从磁盘复制到DRAM中，以替换这个牺牲页

上述功能是由软硬件联合提供的，包括操作系统软件、MMU中的地址翻译硬件和一个存放在物理内存中叫做**页表(page table)**的数据结构。

下图展示了一个页表的基本组织结构。页表就是一个**页表条目(Page Table Entry, PTE)**的数组。PTE由一个有效位和一个n位地址字段组成，有效位表明了该虚拟页当前是否被缓存在DRAM中：

* 如果设置了有效位，
    * 那么地址字段就表示DRAM中相应的物理页的起始位置，这个物理页中缓存了该虚拟页
* 如果没有设置有效位，
    * 那么一个空地址表示这个虚拟页还未被分配
    * 否则，这个地址就指向该虚拟页在**磁盘**上的起始位置

![vm_page_table](./images/vm_page_table.png)

上图展示了一个有8个虚拟也和4个物理页的系统的页表：

* 四个虚拟页(VP1、VP2、VP4和VP7)当前被缓存在DRAM中
* 两个页(VP0和VP5)还未被分配
* 剩下的页(VP3和VP6)已经被分配了，但是当前还未缓存，PTE中的地址指向磁盘上的某个位置

### 页命中

![vm_page_hit](./images/vm_page_hit.png)

### 缺页

![vm_page_fault](./images/vm_page_fault.png)
