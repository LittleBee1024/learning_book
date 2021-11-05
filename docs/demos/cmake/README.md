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
* 开启`verbose`后，可得到完整的编译链接命令
    * 编译命令
    ```bash
    [ 50%] Building CXX object src/CMakeFiles/EXEC.dir/main.cpp.o
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/exec/build/src && /usr/bin/c++   -g -Wall -Werror -Wpedantic -I. -std=c++20 -MD -MT src/CMakeFiles/EXEC.dir/main.cpp.o -MF CMakeFiles/EXEC.dir/main.cpp.o.d -o CMakeFiles/EXEC.dir/main.cpp.o -c /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/exec/src/main.cpp
    ```
    * 链接命令
    ```bash
    [100%] Linking CXX executable EXEC
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/exec/build/src && /usr/bin/cmake -E cmake_link_script CMakeFiles/EXEC.dir/link.txt --verbose=1
    /usr/bin/c++ -g -Wl,-Map=output.map,--print-memory-usage -L. CMakeFiles/EXEC.dir/main.cpp.o -o EXEC 
    Memory region         Used Size  Region Size  %age Used
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

## 动态库工程
动态库通过`add_library`命令添加目标，而EXEC工程通过`add_executable`添加工程。除了这个区别，其他配置两者基本一致。

### [动态库配置文件](./code/libso/src/CMakeLists.txt)
* 为了方便查找动态库的头文件，通过`target_include_directories`命令，将目录文件输出
    ```
    file(GLOB_RECURSE SRC_CPP ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
    # Set SHARED to build so library
    add_library(mylib SHARED ${SRC_CPP})
    # Set public so that its dependency can get the include directory
    target_include_directories(mylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    ```
* 编译命令
    * 先编译`.o`文件
    * 再将所有`.o`文件链接成`.so`文件
    ```bash
    [ 25%] Building CXX object src/CMakeFiles/mylib.dir/hello.cpp.o
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/build/src && /usr/bin/c++ -Dmylib_EXPORTS -I/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/src -g -fPIC -std=c++20 -MD -MT src/CMakeFiles/mylib.dir/hello.cpp.o -MF CMakeFiles/mylib.dir/hello.cpp.o.d -o CMakeFiles/mylib.dir/hello.cpp.o -c /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/src/hello.cpp
    [ 50%] Linking CXX shared library libmylib.so
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/build/src && /usr/bin/cmake -E cmake_link_script CMakeFiles/mylib.dir/link.txt --verbose=1
    /usr/bin/c++ -fPIC -g -shared -Wl,-soname,libmylib.so -o libmylib.so CMakeFiles/mylib.dir/hello.cpp.o 
    make[3]: Leaving directory '/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/build'
    ```

### [动态库测试配置文件](./code/libso/test/CMakeLists.txt)
* 通过`target_link_libraries`命令就可以链接到动态库，由于动态库工程已经将其头文件目录`PUBLIC`输出，所以此处能找到头文件
    ```makefile
    target_link_libraries(myTest PRIVATE
    mylib
    )
    ```
* 编译命令
    ```bash
    [ 75%] Building CXX object test/CMakeFiles/myTest.dir/main.cpp.o
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/build/test && /usr/bin/c++  -I/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/src -g -std=c++20 -MD -MT test/CMakeFiles/myTest.dir/main.cpp.o -MF CMakeFiles/myTest.dir/main.cpp.o.d -o CMakeFiles/myTest.dir/main.cpp.o -c /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/test/main.cpp
    [100%] Linking CXX executable myTest
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/build/test && /usr/bin/cmake -E cmake_link_script CMakeFiles/myTest.dir/link.txt --verbose=1
    /usr/bin/c++ -g CMakeFiles/myTest.dir/main.cpp.o -o myTest  -Wl,-rpath,/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/build/src ../src/libmylib.so 
    make[3]: Leaving directory '/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/libso/build'
    ```


## 静态库工程


## 自定义工程

## 参考
* [cmake常见变量](https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/Useful-Variables)
* [gcc常见编译选项](https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html)