# DWARF

> [《Introduction to the DWARF Debugging Format》 - Michael J. Eager](https://dwarfstd.org/doc/Debugging%20using%20DWARF-2012.pdf)，[《Exploring the DWARF debug format information》](https://developer.ibm.com/articles/au-dwarf-debug-format/)以及[《How debuggers work: Part 3 - Debugging information》](https://eli.thegreenplace.net/2011/02/07/how-debuggers-work-part-3-debugging-information) 的读书笔记，对于DWARF的完整描述可参考[官网](https://dwarfstd.org/)，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/debug_hacks/dwarf/code)中找到


## 简介
DWARF是一种调试文件格式，用于在调试阶段提供源文件的信息。DWARF由下面的段组成。其中，`.debug_info`段记录了最主要的调试信息。

| DWARF段 | 描述 |
| --- | --- |
| .debug_abbrev | Abbreviations used in the .debug_info section |
| .debug_aranges | Lookup table for mapping addresses to compilation units |
| .debug_frame | Call frame information |
| .debug_info | Core DWARF information section |
| .debug_line | Line number information |
| .debug_loc | Location lists used in the DW_AT_location attributes |
| .debug_macinfo | Macro information |
| .debug_pubnames | Lookup table for global objects and functions |
| .debug_pubtypes | Lookup table for global types |
| .debug_ranges | Address ranges used in the DW_AT_ranges attributes |
| .debug_str | String table used in .debug_info |
| .debug_types | Type descriptions |

例如，[例子"debug_format"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/debug_hacks/dwarf/code/debug_format)中的可执行文件中，就包括了如下调试信息：
```bash
> objdump -h main | grep .debug
 27 .debug_aranges 00000040  0000000000000000  0000000000000000  0000303b  2**0
 28 .debug_info   00000430  0000000000000000  0000000000000000  0000307b  2**0
 29 .debug_abbrev 000001aa  0000000000000000  0000000000000000  000034ab  2**0
 30 .debug_line   00000288  0000000000000000  0000000000000000  00003655  2**0
 31 .debug_str    00004e82  0000000000000000  0000000000000000  000038dd  2**0
 32 .debug_ranges 00000030  0000000000000000  0000000000000000  0000875f  2**0
 33 .debug_macro  000012ae  0000000000000000  0000000000000000  0000878f  2**0
```

### DIE
DIE(Debugging Infromation Entry)是DWARF中最基本的单元。每个DIE由一个标签(tag)和若干个属性(atributes)描述。并且，每个DIE都有一个父亲(parent DIE)和多个兄弟(sibling DIEs)，组织成树的形式。

| DIE标签 | 描述 |
| --- | --- |
| DW_TAG_compile_unit | Represents the compile unit information |
| DW_TAG_class_type | Represents the class name and type information |
| DW_TAG_structure_type | Represents the structure name and type information |
| DW_TAG_union_type | Represents the union name and type information |
| DW_TAG_enumeration_type | Represents the enum name and type information |
| DW_TAG_typedef | Represents the typedef name and type information |
| DW_TAG_array_type | Represents the array name and type information |
| DW_TAG_subrange_type | Represents the array size information |
| DW_TAG_inheritance | Represents the inherited class name and type information |
| DW_TAG_member | Represents the members of class |
| DW_TAG_subprogram | Represents the function name information |
| DW_TAG_formal_parameter | Represents the function arguments' information |

| DIE属性 | 描述 |
| --- | --- |
| DW_AT_name | Represents the name string |
| DW_AT_type | Represents the type information |
| DW_AT_artificial | Is set when it is created by compiler |
| DW_AT_sibling | Represents the sibling location information |
| DW_AT_data_member_location | Represents the location information |
| DW_AT_virtuality | Is set when it is virtual |

## 调试信息
`readelf --debug-dump=info <elf>`命令可打印ELF文件中的".debug_info"内容，其他段的内容也可以通过`readelf`的相关命令获取。下面通过[例子"debug_format"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/debug_hacks/dwarf/code/debug_format)，介绍各种类型的调试信息，[源文件"main.cpp"](./code/debug_format/main.cpp)的内容如下：
```cpp
#include <stdio.h>

typedef int int_type;
enum myenum { Jan = 1, Feb = 2};

class base
{
public:
    int basemember;
};

class myclass : public base
{
public:
    int a;
    char b[8];
    int_type c;
    myenum myclass_myenum;
    void display(int x)
    {
        x = 4;
    }
};

int main()
{
    int test = 3;
    myclass mc;
    mc.display(test);
}
```

### 编译单元信息
```bash title=".debug_info段中的编译单元信息"
 <0><b>: Abbrev Number: 1 (DW_TAG_compile_unit)
    <c>   DW_AT_producer    : (indirect string, offset: 0x3d42): GNU C++14 9.4.0 -mtune=generic -march=x86-64 -g3 -O0 -fno-inline -fasynchronous-unwind-tables -fstack-protector-strong -fstack-clash-protection -fcf-protection
    <10>   DW_AT_language    : 4	(C++)
    <11>   DW_AT_name        : (indirect string, offset: 0x1138): main.cpp
    <15>   DW_AT_comp_dir    : (indirect string, offset: 0x2fc8): /home/ben/GitHub/learning_book/docs/booknotes/debug_hacks/dwarf/code/debug_format
    <19>   DW_AT_ranges      : 0x0
    <1d>   DW_AT_low_pc      : 0x0
    <25>   DW_AT_stmt_list   : 0x0
    <29>   DW_AT_GNU_macros  : 0x0
```

* `DW_TAG_compile_unit` - 编译单元标签
    * `DW_AT_name` - 编译单元的文件名为"main.cpp"
    * `DW_AT_comp_dir` - 编译单元文件夹

### 类型信息

[源文件"main.cpp"](./code/debug_format/main.cpp)中的`typedef int int_type`，在".debug_info"段中对应信息如下：

```bash title=".debug_info段中的typedef"
 <1><65>: Abbrev Number: 5 (DW_TAG_base_type)
    <66>   DW_AT_byte_size   : 4
    <67>   DW_AT_encoding    : 5	(signed)
    <68>   DW_AT_name        : int
 ...
 <1><2f5>: Abbrev Number: 2 (DW_TAG_typedef)
    <2f6>   DW_AT_name        : (indirect string, offset: 0xd73): int_type
    <2fa>   DW_AT_decl_file   : 1
    <2fb>   DW_AT_decl_line   : 3
    <2fc>   DW_AT_decl_column : 13
    <2fd>   DW_AT_type        : <0x65>
```

* `DW_TAG_base_type` - 基本类型
    * DWARF规定了常见基本类型的信息，如长度为4bytes的`int`类型
* `DW_TAG_typedef` - `typedef`标签
    * `DW_AT_name` - `typedef`自定义的类型名：`int_type`
    * `DW_AT_type` - `typedef`的原始类型，此处的`<0x65>`是`int`类型的位置
    * `DW_AT_decl_file` - 源代码文件编号，通过`readelf --debug-dump=line main`中的`The File Name Table`可查看对应文件名。例如，"main.cpp"的编号为1，"stddef.h"的编号为2
    * `DW_AT_decl_line` - 源代码文件中的行号

[源文件"main.cpp"](./code/debug_format/main.cpp)中的`enum myenum { Jan = 1, Feb = 2}`，在".debug_info"段中对应信息如下：

```bash title=".debug_info段中的enum"
 <1><301>: Abbrev Number: 16 (DW_TAG_enumeration_type)
    <302>   DW_AT_name        : (indirect string, offset: 0x546): myenum
    <306>   DW_AT_encoding    : 7	(unsigned)
    <307>   DW_AT_byte_size   : 4
    <308>   DW_AT_type        : <0x40>
    <30c>   DW_AT_decl_file   : 1
    <30d>   DW_AT_decl_line   : 4
    <30e>   DW_AT_decl_column : 6
    <30f>   DW_AT_sibling     : <0x320>
 <2><313>: Abbrev Number: 17 (DW_TAG_enumerator)
    <314>   DW_AT_name        : Jan
    <318>   DW_AT_const_value : 1
 <2><319>: Abbrev Number: 17 (DW_TAG_enumerator)
    <31a>   DW_AT_name        : Feb
    <31e>   DW_AT_const_value : 2
```

* `DW_TAG_enumeration_type` - 枚举类型标签
    * 包括枚举成员的类型和大小
* `DW_TAG_enumerator` - 枚举常量标签
    * 包括枚举成员的名字和值


### 类信息
[源文件"main.cpp"](./code/debug_format/main.cpp)中的基类`bash`，在".debug_info"段中对应信息如下：
```bash title=".debug_info段中的基类信息"
 <1><320>: Abbrev Number: 18 (DW_TAG_class_type)
    <321>   DW_AT_name        : (indirect string, offset: 0x230e): base
    <325>   DW_AT_byte_size   : 4
    <326>   DW_AT_decl_file   : 1
    <327>   DW_AT_decl_line   : 6
    <328>   DW_AT_decl_column : 7
    <329>   DW_AT_sibling     : <0x33c>
 <2><32d>: Abbrev Number: 19 (DW_TAG_member)
    <32e>   DW_AT_name        : (indirect string, offset: 0x25db): basemember
    <332>   DW_AT_decl_file   : 1
    <333>   DW_AT_decl_line   : 9
    <334>   DW_AT_decl_column : 8
    <335>   DW_AT_type        : <0x65>
    <339>   DW_AT_data_member_location: 0
    <33a>   DW_AT_accessibility: 1	(public)
 <2><33b>: Abbrev Number: 0
```

* `DW_TAG_class_type` - 类标签
    * 基类的名字是`base`，大小为4bytes
    * `DW_AT_sibling` - 兄弟标签的位置，表明当前位置到`<0x33c>`之前的内容都是当前类的信息
* `DW_TAG_member` - 类成员标签
    * 基类成员的名字是`basemember`
    * 基类成员的类型定义在`<0x65>`处，是大小为4bytes的`int`类型
    * `DW_AT_accessibility` - 类成员的访问属性

源文件中，`myclass`类继承了`base`类，继承类在".debug_info"段中对应信息如下：

```bash title=".debug_info段中的继承类信息"
 <1><33c>: Abbrev Number: 18 (DW_TAG_class_type)
    <33d>   DW_AT_name        : (indirect string, offset: 0x4380): myclass
    <341>   DW_AT_byte_size   : 24
    <342>   DW_AT_decl_file   : 1
    <343>   DW_AT_decl_line   : 12
    <344>   DW_AT_decl_column : 7
    <345>   DW_AT_sibling     : <0x39f>
 <2><349>: Abbrev Number: 20 (DW_TAG_inheritance)
    <34a>   DW_AT_type        : <0x320>
    <34e>   DW_AT_data_member_location: 0
    <34f>   DW_AT_accessibility: 1	(public)
 <2><350>: Abbrev Number: 21 (DW_TAG_member)
    <351>   DW_AT_name        : a
    <353>   DW_AT_decl_file   : 1
    <354>   DW_AT_decl_line   : 15
    <355>   DW_AT_decl_column : 8
    <356>   DW_AT_type        : <0x65>
    <35a>   DW_AT_data_member_location: 4
    <35b>   DW_AT_accessibility: 1	(public)
    ...
```

* `DW_TAG_inheritance` - 继承信息标签
    * 继承了定义在`<0x320>`处的基类`base`
* 其他内容和普通类一样

### 函数信息
[源文件"main.cpp"](./code/debug_format/main.cpp)中的`void myclass::display(int x)`，在".debug_info"段中对应信息如下：
```bash title=".debug_info段中的函数信息"
 <2><382>: Abbrev Number: 22 (DW_TAG_subprogram)
    <383>   DW_AT_external    : 1
    <383>   DW_AT_name        : (indirect string, offset: 0x3c7b): display
    <387>   DW_AT_decl_file   : 1
    <388>   DW_AT_decl_line   : 19
    <389>   DW_AT_decl_column : 9
    <38a>   DW_AT_linkage_name: (indirect string, offset: 0x1141): _ZN7myclass7displayEi
    <38e>   DW_AT_accessibility: 1	(public)
    <38f>   DW_AT_declaration : 1
    <38f>   DW_AT_object_pointer: <0x393>
 <3><393>: Abbrev Number: 23 (DW_TAG_formal_parameter)
    <394>   DW_AT_type        : <0x3af>
    <398>   DW_AT_artificial  : 1
 <3><398>: Abbrev Number: 24 (DW_TAG_formal_parameter)
    <399>   DW_AT_type        : <0x65>
```

* `DW_TAG_subprogram` - 函数标签
    * 函数名为`display`
    * `display`函数定义在"main.cpp"中的第19行
    * `_ZN7myclass7displayEi`是`myclass::display`的函数编码后的符号
* `DW_TAG_formal_parameter` - 函数参数标签
    * 参数1是`base`类的指针类型，位于`<0x3af>`
    * 参数2是`int`类型，位于`<0x65>`

### 行信息
`readelf --debug-dump=decodedline main`命令可以".debug_line"段中的信息：

```bash title=".debug_line段中的行信息"
File name                            Line number    Starting address    View    Stmt
main.cpp                                      19              0x1198               x
main.cpp                                      21              0x11a7               x
main.cpp                                      22              0x11ae               x
main.cpp                                      22              0x11b1               x

main.cpp                                      26              0x1149               x
main.cpp                                      26              0x1155               x
main.cpp                                      27              0x1164               x
main.cpp                                      29              0x116b               x
main.cpp                                      30              0x117c               x
main.cpp                                      30              0x1197               x
```

".debug_line"段建立了源文件行号和程序地址的一一对应关系：

* 当在某一行号设置断点时，调试器可通过".debug_line"段，根据源文件行号找到对应的地址
* 当某指令发生错误时，调试器可通过".debug_line"段，根据出错地址定位到源文件行号
