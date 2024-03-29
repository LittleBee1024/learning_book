# 优化程序性能

> [《深入理解计算机系统》 - Randal E. Bryant - 第三版](https://1drv.ms/b/s!AkcJSyT7tq80bJdqo_mT5IeFTsg?e=W297XG)，第五章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/05/code)中找到

## 编译器的优化能力和局限性

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

## 循环优化

[例子"combine"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/csapp/05/code/combine)对比了不同循环实现的运行时间：
```cpp
// 在循环体中调用`vec_length`
void combine1(vec_ptr v, data_t *dest)
{
    for (long i = 0; i < vec_length(v); i++)
    {
        data_t val;
        get_vec_element(v, i, &val);
        *dest = (*dest) + val;
    }
}

// 利用变量len减少`vec_length`的调用次数
void combine2(vec_ptr v, data_t *dest)
{
    long len = vec_length(v);
    for (long i = 0; i < len; i++)
    {
        data_t val;
        get_vec_element(v, i, &val);
        *dest = (*dest) + val;
    }
}

// 利用`get_vec_start`减少`get_vec_element`的调用次数
void combine3(vec_ptr v, data_t *dest)
{
    long len = vec_length(v);
    data_t *data = get_vec_start(v);
    for (long i = 0; i < len; i++)
    {
        *dest = *dest + data[i];
    }
}

// 将循环的结果累积在临时变量acc中
void combine4(vec_ptr v, data_t *dest)
{
    long len = vec_length(v);
    data_t *data = get_vec_start(v);
    data_t acc = *dest;
    for (long i = 0; i < len; i++)
    {
        acc = acc + data[i];
    }
    *dest = acc;
}
```

从`combine1`到`combine4`，我们逐步降低了过程调用的开销。在不同的编译器优化等级下，四个函数的运行时间对比如下：

* `O0`优化等级
    * `combine1/2`运行时间相近
    * `combine3/4`运行时间相近
    * `combine3/4`运行时间优于`combine1/2`
* `O1`优化等级
    * `combine1/2/3`运行时间相近
    * `combine4`运行时间优于其他
* `O2`优化等级
    * 四个函数的相对趋势和`O0`优化等级类似，但是所有函数运行时间都缩短了

通过对比实验结果，可得出以下结论：

* 通过临时变量累积每次循环的结果，可减少对内存的访问次数，从而优化运行时间
* 编译器会自行优化函数调用的次数(如：通过将函数内联)，因此可将此种优化方式交给编译器

### 循环展开并累积在多个值中

```cpp
// 循环展开
void combine5(vec_ptr v, data_t *dest)
{
    long len = vec_length(v);
    data_t *data = get_vec_start(v);
    data_t acc = *dest;

    long i = 0;
    long len_floor = len - (len % 2);
    // combine 2 elements at a time
    for (; i < len; i += 2)
    {
        acc = (acc + data[i]) + data[i+1];
    }
    // finish any remaining elements
    for (; i < len; i++)
    {
        acc += data[i];
    }
    *dest = acc;
}

// 累积在acc0和acc1两个变量中
void combine6(vec_ptr v, data_t *dest)
{
    long len = vec_length(v);
    data_t *data = get_vec_start(v);
    data_t acc0 = *dest;
    data_t acc1 = 0;

    long i = 0;
    long len_floor = len - (len % 2);
    // combine 2 elements at a time
    for (; i < len_floor; i += 2)
    {
        acc0 += data[i];
        acc1 += data[i+1];
    }
    // finish any remaining elements
    for (; i < len; i++)
    {
        acc0 += data[i];
    }

    *dest = acc0 + acc1;
}

// 先结合，后累积
void combine7(vec_ptr v, data_t *dest)
{
    long len = vec_length(v);
    data_t *data = get_vec_start(v);
    data_t acc = *dest;

    long i = 0;
    long len_floor = len - (len % 2);
    // combine 2 elements at a time
    for (; i < len; i += 2)
    {
        acc += (data[i] + data[i+1]);
    }
    // finish any remaining elements
    for (; i < len; i++)
    {
        acc += data[i];
    }
    *dest = acc;
}
```
对比实验结果，可发现：

* `combine5`通过循环展开，一次循环累积两个只。但是由于关键路径并没有变短，优化效果并不明显
* `combine6`在`combine5`的基础上增加了一个临时变量，将关键路径缩短了一般，运行时间缩短明显
* `combine7`虽然没有增加临时变量，但是在循环中只累积一次，同样缩短了关键路径，提高了效率

总之，循环展开和并行地累积在多个值中，是提高程序性能的可靠方法。

## 性能限制因素

一个程序的执行时间与其“关键路径长度”和“吞吐量界限”有关：

* 关键路径
    * 指明了执行该程序所需时间的一个基本下界。如果程序中有某条数据相关链，这条链上的所有延迟之和等于`T`，那么这个程序至少需要`T`个周期才能执行完
* 吞吐量
    * 假设一个程序一共需要N个某种运算的计算，而微处理器只有C个能执行这个操作的功能单元，并且这些单元的发射时间为I。那么这个程序的执行至少需要`N*I/C`个周期

### 寄存器溢出

如果我们的并行度超过了可用的寄存器数量，那么编译器会诉诸寄存器溢出，将某些临时值存放到内存中，从而降低了运行效率。

### 分支预测错误处罚

保证分支预测处罚不会阻碍程序效率的通用原则有：

* 不要过分关心可预测的分支
    * 现代处理器中的分支预测逻辑非常善于预测分支。例如：
        * 上例中的`combine2`和`combine3`运行时间相近，原因是每次迭代的判断语句高度可预测
        * 同理，边界检测所需的额外计算并不会对关键路径产生太大的影响
* 书写适合用条件传送实现的代码
    * 对于本质上无法预测的情况(如一个数是正数还是负数)，如果能使用条件数据传送而不使用条件控制转移的代码，可大大提高效率。例如，下例中的`minmax2`比`minmax`效率更高：
    ```cpp
    void minmax(long a[], long b[], long n)
    {
        for (long i = 0; i < n; i++)
        {
            if (a[i] > b[i])
            {
                long t = a[i];
                a[i] = b[i];
                b[i] = t;
            }
        }
    }

    void minmax2(long a[], long b[], long n)
    {
        for (long i = 0; i < n; i++)
        {
            long min = a[i] < b[i] ? a[i] : b[i];
            long max = a[i] < b[i] ? b[i] : a[i];
            a[i] = min;
            b[i] = max;
        }
    }
    ```

## 总结

优化程序的基本策略：

* 高级设计
    * 避免使用性能糟糕的数据结构和算法
* 基本编码原则
    * 消除连续的函数调用
        * 如果可能，将计算移动到循环外
    * 消除不必要的内存引用
        * 引入临时变量保存中间结果，在最后才将结果放到全局变量中
* 低级优化
    * 展开循环，降低开销
    * 通过使用多个积累变量和重新结合等技术，找到方法提高指令级并行
    * 重写条件操作，使得编译采用条件数据传送



