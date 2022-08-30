# Tcl/Tk in C

> 本文包含将`tcl/tk`集成进C代码的简单样例。样例依赖`tcl/tk8.6`开发库，如发现编译错误，需要安装`tcl-dev`和`tk-dev`，并在`Makefile`中修改对应的头文件和库文件路径。例如，在`ubuntu20`下通过`sudo apt install tcl tk tcl-dev tk-dev`即可完成安装。所有样例可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/topics/tcl_tk_in_c/code)中找到，大部分代码都参考了[《Practical Programming in Tcl and Tk》](https://1drv.ms/u/s!AkcJSyT7tq80feibH0jz3d0nn7s)中的例子

## C接口

### Tcl_Main
[例子"Tcl_Main"](https://github.com/LittleBee1024/learning_book/tree/main/docs/topics/tcl_tk_in_c/code/api/Tcl_Main)在`main`函数中调用了`Tcl_Main`启动了一个TCL解释器，会解析并运行`argv`中传入的TCL文件"hello.tcl"，在终端打印“hello”字样：

```cpp title="main.c" hl_lines="3 11"
int Tcl_AppInit(Tcl_Interp *interp)
{
    if (Tcl_Init(interp) == TCL_ERROR)
        return TCL_ERROR;

    return TCL_OK;
}

int main(int argc, char *argv[])
{
    Tcl_Main(argc, argv, Tcl_AppInit);
    return 0;
}
```
```bash
# hello.tcl:
#   puts "hello"
> ./main hello.tcl
hello
```

### Tk_Main
[例子"Tk_Main"](https://github.com/LittleBee1024/learning_book/tree/main/docs/topics/tcl_tk_in_c/code/api/Tk_Main)在`main`函数中调用了`Tk_Main`启动了一个TCL/TK解释器，会解析并运行`argv`中传入的TCL/TK文件"hello.tcl"，打开一个带“Hello, world!”字样的按钮：
```cpp title="main.c" hl_lines="3 5 13"
int AppInit(Tcl_Interp *interp)
{
    if (Tcl_Init(interp) == TCL_ERROR)
        return TCL_ERROR;
    if (Tk_Init(interp) == TCL_ERROR)
        return TCL_ERROR;

    return TCL_OK;
}

int main(int argc, char *argv[])
{
    Tk_Main(argc, argv, AppInit);
    return 0;
}
```
```bash
# hello.tcl:
#   button .b -text "Hello, world!" -command exit
#   pack .b
> ./main hello.tcl
```
![tk_hello](./images/tk_hello.png)

### Tcl_CreateCommand
[例子"Tcl_CreateCommand"](https://github.com/LittleBee1024/learning_book/tree/main/docs/topics/tcl_tk_in_c/code/api/Tcl_CreateCommand)利用C库的`random`函数，创建了一个TCL命令`random`，可在TCL文件中使用此命令，完成了C代码和TCL代码的交互：

```cpp title="main.c" hl_lines="15 21"
// 参考:《Practical Programming in Tcl and Tk》第47章的"A C Command Procedure"
int RandomCmd(ClientData clientData, Tcl_Interp *interp, int argc, CONST char *argv[])
{
    int range = 0;
    char buffer[20];
    ...
    printf("[  C] Process random()\n");
    int rand = random();
    if (range != 0)
    {
        rand = rand % range;
    }
    sprintf(buffer, "%d", rand);
    // TCL_VOLATILE用于局部遍历的析构，将结果以字符串的形式返回
    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
    return TCL_OK;
}

int Random_Init(Tcl_Interp *interp)
{
   Tcl_CreateCommand(interp, "random", RandomCmd, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
   return TCL_OK;
}

int main(int argc, char *argv[])
{
   Tcl_Main(argc, argv, Random_Init);
   return 0;
}
```
```bash
# cmd.tcl:
#   set result [random 5]
#   puts "\[TCL\] random result: ${result}"
#   set result [random 100]
#   puts "\[TCL\] random result: ${result}"
> ./main cmd.tcl
[  C] Process random()
[TCL] random result: 3
[  C] Process random()
[TCL] random result: 86
```

### Tcl_EvalFile
[例子"Tcl_EvalFile"](https://github.com/LittleBee1024/learning_book/tree/main/docs/topics/tcl_tk_in_c/code/api/Tcl_EvalFile)

### Tcl_Obj
[例子"Tcl_Obj"](https://github.com/LittleBee1024/learning_book/tree/main/docs/topics/tcl_tk_in_c/code/api/Tcl_Obj)


## Tk组件

## 自定义命令

## TCL Shell实例

## Exec Log实例

## Browser实例

## 参考
* [《Practical Programming in Tcl and Tk》- 4th, Brent Welch, Ken Jones](https://1drv.ms/u/s!AkcJSyT7tq80feibH0jz3d0nn7s)，"Part VI: C Programming"
* [《Tcl and the Tk Toolkit》- John K. Ousterhout](https://1drv.ms/b/s!AkcJSyT7tq80fFnaE5hYkCST6oI)，"PART III Writing Tcl Applications in C"
* [《Tcl/Tk A Developers Guide》- Clif Flynt](https://1drv.ms/b/s!AkcJSyT7tq80e-BG7QjXcXKSlEA)，"Chapter 15 Extending Tcl"

