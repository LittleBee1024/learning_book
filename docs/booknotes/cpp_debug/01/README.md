# 调试符号和调试器

> [《高效C/C++调试》 - 严琦](http://www.tup.tsinghua.edu.cn/Wap/tsxqy.aspx?id=10204101)，第一章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/01/code)中找到

## 调试符号

常见的调试符号有以下几类：

* 全局函数和变量
* 源文件和行信息
* 类型信息
* 静态函数和局部变量
* 架构和编译器依赖信息

### DWARF格式

[DWARF格式](https://dwarfstd.org/)是Linux系统中的标准调试符号格式。DWARF以树形结构组织调试符号。每一个树节点都是一个调试信息记录(Debug Information Entry, DIE)。

下面我们以[foo.cpp](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/cpp_debug/01/code/dwarf/foo.cpp)为例介绍DWARF格式：

```cpp linenums="1"
int gInt = 1;
int GlobalFunc(int i)
{
    return i + gInt;
}
```

我们可以用`readelf --debug-dump foo.o`命令查看调试符号的各种信息。调试符号被划分为多个节(section)：

* `.debug_abbrev`节
    * 缩略表中记录DIE的模板，用于真实DIE项的索引
* `.debug_info`节
    * 包含了调试符号的核心内容，其中每个DIE都指定了其在缩略表中的索引
* `.debug_line`节
    * 用于将指令地址映射到源代码行号
* `.eh_frame`节(或`.debug_frame`节)
    * 描述了函数的栈帧和寄存器的分配方式

以`GlobalFunc`函数的参数`int i`为例，其在汇编代码"foo.s"中编码如下：

```asm
.uleb128 0x5
.string	"i"
.byte	0x1
.byte	0x2
.byte	0x14
.long	0x44
.uleb128 0x2
.byte	0x91
.sleb128 -20
.byte	0
```

参数`int i`的DIE在`.debug_info`段中内容如下：
```
 <2><6d>: Abbrev Number: 5 (DW_TAG_formal_parameter)
    <6e>   DW_AT_name        : i
    <70>   DW_AT_decl_file   : 1
    <71>   DW_AT_decl_line   : 2
    <72>   DW_AT_decl_column : 20
    <73>   DW_AT_type        : <0x44>
    <77>   DW_AT_location    : 2 byte block: 91 6c 	(DW_OP_fbreg: -20)
```

其对应的原始数据可通过`objdump -s --section=.debug_info foo.o`命令查看：

* `05 6900 01 02 14 44 00000002 916c 00`

参数`int i`的DIE含义包括：

* 这个DIE在缩略表中的索引是5
* 参数名称是“i”
* 它出现在第1个文件中
* 它位于该文件的第2行，第20列
* 参数的类型有另一个DIE(引用为0x44)来描述
* 参数的存储位置由接下来的2字节指示，这对应于相对于寄存器fbreg的偏移量-20
* 最后这个DIE以一共0字节结束
