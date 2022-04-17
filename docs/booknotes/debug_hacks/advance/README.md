# 高级调试技术

> [《Debug Hacks--深入调试的技术和工具》 - 吉冈弘隆](https://1drv.ms/b/s!AkcJSyT7tq80clf1-pjOCricrUs?e=xIJL0b)，第四章和第六章的读书比较，本文中所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/debug_hacks/advance/code)中找到

## GOT和PLT
### 工作原理

* GOT(Global Offset Table)
    * GOT是保存库函数地址的区域，程序运行时，用到的库函数地址会设置到该区域
* PLT(Procedure Linkage Table)
    * PLT时调用库函数时的小型代码集合，程序可以像调用自己的用户函数一样调用这些小型代码。这些代码只是跳转到GOT中设置的值而已。如果GOT中尚未设置调用函数的地址，就将地址设置到GOT中再跳转。

动态库函数每次运行的地址都不同，利用GOT和PLT机制，进程可将库函数看作一个固定不变的地址。

### 实例
在[示例代码](./code/GOT_PLT/main.c)的汇编代码中，

* `foo`库函数的调用代码是`callq  0x555555555050 <foo@plt>`，跳转到`foo`库函数的PLT地址
* `0x555555555050`对应的汇编代码第一句是跳转指令，跳转地址保存在`0x555555557fd0`地址中（代码中的`*`是取值的意思）
* `0x555555557fd0`地址的内容值是`0xf7fc30f9`，这是一个无法访问的内核地址，结合书中的例子（可能是GCC版本不同，书中的例子此处是可以访问的），`0xf7fc30f9`地址是`foo`库函数的GOT入口
    * 如果GOT尚未设置过，后面的指令会将`foo`库函数的地址填入GOT对应的位置，再跳转
    * 如果GOT已经设置过，会直接找到GOT中`foo`函数的地址，直接跳转
* 由于`foo`库函数的PLT地址是固定的，从主进程的角度看，其调用的`foo`库函数的地址，每次运行都不变，都是`0x555555555050`
```asm
(gdb) disas main
Dump of assembler code for function main:
=> 0x0000555555555158 <+0>:     endbr64 
   0x000055555555515c <+4>:     push   %rbp
   0x000055555555515d <+5>:     mov    %rsp,%rbp
   0x0000555555555160 <+8>:     sub    $0x10,%rsp
   0x0000555555555164 <+12>:    callq  0x555555555149 <bar>
   0x0000555555555169 <+17>:    mov    %eax,-0xc(%rbp)
   0x000055555555516c <+20>:    mov    $0x0,%eax
   0x0000555555555171 <+25>:    callq  0x555555555050 <foo@plt>
   0x0000555555555176 <+30>:    mov    %eax,-0x8(%rbp)
   0x0000555555555179 <+33>:    mov    -0xc(%rbp),%edx
   0x000055555555517c <+36>:    mov    -0x8(%rbp),%eax
   0x000055555555517f <+39>:    add    %edx,%eax
   0x0000555555555181 <+41>:    mov    %eax,-0x4(%rbp)
   0x0000555555555184 <+44>:    mov    $0x0,%eax
   0x0000555555555189 <+49>:    leaveq 
   0x000055555555518a <+50>:    retq 

(gdb) disas 0x555555555050
Dump of assembler code for function rand@plt:
   0x0000555555555050 <+0>:     endbr64 
   0x0000555555555054 <+4>:     bnd jmpq *0x2f75(%rip)        # 0x555555557fd0 <foo@got.plt>
   0x000055555555505b <+11>:    nopl   0x0(%rax,%rax,1)

(gdb) p/x *0x555555557fd0
$1 = 0xf7fc30f9
```
 