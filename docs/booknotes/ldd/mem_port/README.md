# 与硬件通信

> [《Linux设备驱动程序》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80d6mS7pO12K6Qb30)的第9章，[《Linux设备驱动开发详解》 - 宋宝华 ](https://1drv.ms/b/s!AkcJSyT7tq80eFABEg8fSOajqHk)的第11章，的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/mem_port/code)中找到，本文中的实验涉及与硬件的通信，可能会由于硬件的原因，无法得到期望的结果

## I/O端口和I/O内存
设备通常会提供一组寄存器来控制设备、读写设备和获取设备状态，即控制寄存器、数据寄存器和状态寄存器。这些寄存器可能位于**I/O空间**中，也可能位于**内存空间**中。

* 当位于I/O空间时，被称为I/O端口
* 当位于内存空间时，被称为I/O内存

![mem_port](./images/mem_port.png)

### I/O空间
在x86处理器中存在着I/O空间，而大多数嵌入式控制器(如ARM、PowerPC等)中并不提供I/O空间，只存在内存空间。I/O空间通过特定的指令`in`和`out`来访问。

在linux中`/proc/ioports`记录了系统分配的所有I/O端口。例如，下面所列出的端口都已经被对应的驱动所占用：
```bash
> sudo cat /proc/ioports
0000-0cf7 : PCI Bus 0000:00
  0000-001f : dma1
  0020-0021 : PNP0001:00
    0020-0021 : pic1
  0040-0043 : timer0
  0050-0053 : timer1
  0060-0060 : keyboard
  0061-0061 : PNP0800:00
  0064-0064 : keyboard
  0070-0071 : rtc0
  0080-008f : dma page reg
  00a0-00a1 : PNP0001:00
    00a0-00a1 : pic2
  00c0-00df : dma2
  00f0-00ff : fpu
  0170-0177 : 0000:00:07.1
    0170-0177 : ata_piix
  01f0-01f7 : 0000:00:07.1
    01f0-01f7 : ata_piix
  0376-0376 : 0000:00:07.1
    0376-0376 : ata_piix
  03c0-03df : vga+
  03f6-03f6 : 0000:00:07.1
    03f6-03f6 : ata_piix
  04d0-04d1 : PNP0001:00
  0cf0-0cf1 : pnp 00:00
```

`request_region`函数用于在驱动程序中注册I/O端口。如果函数失败，可能端口已经被其他驱动所占用。

```cpp
#include <linux/ioport.h>
// 注册I/O端口
//  first - 端口起始地址
//  n - 需注册的端口个数
//  name - 设备名称，显示在/proc/ioports中
struct resource *request_region(unsigned long first, unsigned long n, const char *name);
```

#### 用户空间读写

### 内存空间
内存空间可以直接通过地址、指针来访问。


#### 用户空间读写


