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
