# Flex和Bison

> Flex和Bison工具的样例，所有样例可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/topics/flex_bison/code)中找到

## 简介

在编译器领域，分为两个部分：

* 词法分析(lexical analysis, 或scanning)
    * 把输入分割成一个个有意义的词块
* 语法分析(syntax analysis, 或parsing)
    * 确定这些记号是如何彼此关联的

Flex用于词法分析，而Bison用于语法分析。

### Flex

Flex的目的是将输入分割成一个个有意义的词块，用户通过调用Flex自动生成的`yylex`函数，即可启动词法分析工作。下图是其常见的工作流程：

* 通过`yyin`读入用户的输入，默认情况下是标准输入`stdin`，用户可以自定义为其他输入源
* 将输入和用户定义的词法规则(正则表达式)进行比较`match`
* 如果输入匹配某条规则，触发用户定义的动作`action`，用户可通过`yytext`获取匹配的字符串
* 如果输入没有匹配的规则，或则`action`完成(没有退出`yylex`函数的语句)，重新处理新的输入
* `yylex`函数会一直循环上面的步骤，直到没有更多的输入(遇到EOF)

![flex](./image/flex.png)

当遇到多个规则同时匹配时，Flex遵循以下规则选择最合适的规则：

* 匹配尽可能多给的字符串
* 如果匹配字符串长度相同，选择在程序中更早出现的规则

### Bison

![bison](./image/bison.png)

## 参考

* [《flex与bison》](https://1drv.ms/b/s!AkcJSyT7tq80eo_xy7LTpX6PPs4)
* [Flex Manual](https://www.cs.virginia.edu/~cr4bd/flex-manual/index.html#SEC_Contents)
* [Bison Manual](https://www.gnu.org/software/bison/manual/bison.html)
