# 优化程序性能

> [《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG)，第五章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/05/code)中找到

## 优化编译器的能力和局限性

```cpp
long f();

long func1() {
    return f() + f() + f() + f();
}

long fun2() {
    return 4 * f();
}

// 假如f的代码如下，func1和func2的行为将不一致
long counter = 0;
long f() {
    return counter++;
}
```

上面的例子中，由于`f()`函数存在副作用，编译器不会将`func1`优化成`func2`。针对此情况，GCC会尝试进行“用内联函数替换优化函数调用”：

```cpp
// 将函数调用替换为函数体，对func1进行优化
long func1in() {
    long t = counter++;
    long t = counter++;
    long t = counter++;
    long t = counter++;
    return t;
}
```

GCC只尝试在单个文件中定义的函数的内联。这意味着它将无法应用于常见的情况，即一组库函数在一个文件中被定义，却被其他文件内的函数所调用。

当我们需要需要用符号调试器，对函数进行追踪或设置断点时，最好降低优化等级以阻止编译器执行内联替换。
