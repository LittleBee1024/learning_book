# cmake

> cmake不同工程类型的样例，所有样例可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/demos/cmake/code)中找到

## Exec工程
cmake通过目录下的`CMakeLists.txt`文件，将整个工程串联起来。在主目录的`CMakeLists.txt`文件中，通过`add_subdirectory`命令加入子一级的目录，以此类推。

### [主配置文件](./code/exec/CMakeLists.txt)
* 主配置文件主要配置一些全局信息。例如，项目名称，C++库版本等。同时加入子一级目录。
    ```makefile
    # set the C++ standard
    set(CMAKE_CXX_STANDARD 20)
    # Use STD c++ instead of GNU
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    ```
### [Exec目标配置文件](./code/exec/src/CMakeLists.txt)
* 为了统一，可选择`set_target_properties`命令，为此目标配置所有的编译信息
    * 编译和链接选项通过分号隔离
    * 默认文件输出路径就是`CMAKE_CURRENT_BINARY_DIR`
    ```makefile
    set_target_properties(EXEC PROPERTIES
        COMPILE_OPTIONS "-Wall;-Werror;-Wpedantic;-I."
        LINK_OPTIONS "-Wl,-Map=output.map,--print-memory-usage;-L."
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    ```
### cmake目标
cmake配置完成后，可通过`cmake`生成对应的`Makefile`。此工程的目标有：
```bash
build> make help
The following are some of the valid targets for this Makefile:
... all (the default if no target is provided)
... clean
... depend
... edit_cache
... rebuild_cache
... EXEC
```
其中，`EXEC`是我们定义的目标，其他的是由`cmake`自己生成的。


## 静态库工程
## 动态库工程
## 自定义工程

## 参考
* [cmake常见变量](https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/Useful-Variables)
* [gcc常见编译选项](https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html)