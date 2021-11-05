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
静态库工程和动态工程除了在`add_library`命令中使用`STATIC`选项以外，其他和动态库工程完全一样。

### [静态库配置文件](./code/liba/src/CMakeLists.txt)
* 通过`add_library(mylib STATIC ${SRC_CPP})`配置了静态库目标
* 编译命令
    ```bash
    [ 25%] Building CXX object src/CMakeFiles/mylib.dir/hello.cpp.o
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/liba/build/src && /usr/bin/c++  -I/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/liba/src -g -std=c++20 -MD -MT src/CMakeFiles/mylib.dir/hello.cpp.o -MF CMakeFiles/mylib.dir/hello.cpp.o.d -o CMakeFiles/mylib.dir/hello.cpp.o -c /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/liba/src/hello.cpp
    [ 50%] Linking CXX static library libmylib.a
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/liba/build/src && /usr/bin/cmake -P CMakeFiles/mylib.dir/cmake_clean_target.cmake
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/liba/build/src && /usr/bin/cmake -E cmake_link_script CMakeFiles/mylib.dir/link.txt --verbose=1
    /usr/bin/ar qc libmylib.a CMakeFiles/mylib.dir/hello.cpp.o
    /usr/bin/ranlib libmylib.a
    make[3]: Leaving directory '/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/liba/build'
    ```

## 自定义工程
除了常规目标(EXEC，静态库，动态库)，cmake还提供了一些命令用于用户自定义的目标：

* [`add_custom_command`](https://cmake.org/cmake/help/v3.22/command/add_custom_command.html?highlight=add_custom_command)
    * 用于产生当前目标的依赖文件，本文的例子就用此命令执行了自动生成的代码，用于当前目标的编译
* [`add_custom_target`](https://cmake.org/cmake/help/v3.22/command/add_custom_target.html?highlight=add_custom_target)
    * 创建一个非常规的目标。例如，我们可以用其创建一个`doxygen`目标用来生成代码文档
* [`add_dependencies`](https://cmake.org/cmake/help/v3.22/command/add_dependencies.html?highlight=add_custom_target#add-dependencies)
    * 建立自定义依赖关系。cmake对库依赖会自动建立依赖关系，但是如果我们想建立一个非常规的依赖关系，就需要用到此命令。例如，建立自定义目标的依赖关系
    * 当依赖关系建立好后，系统会按照依赖关系进行按顺序编译

### [自定义目标配置文件](./code/custom/src/CMakeLists.txt)
本例的编译顺序如下，可参考简单的[Makefile](./code/custom/src/Makefile)文件：

* 先通过`python`的`cog`命令自动生成CPP源代码
* 再将`main.cpp`与生成的代码进行编译
* 最终生成一个可执行文件

自定义目标的配置文件如下：
```makefile
set(CODEGEN_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/internal")
set(CODEGEN_SRC "${CODEGEN_BINARY_DIR}/animal.cpp")

add_executable(main main.cpp ${CODEGEN_SRC})

add_custom_command(
   OUTPUT ${CODEGEN_SRC}
   # Copy all files in this folder to $(CODEGEN_BINARY_DIR)
   COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_SOURCE_DIR}/internal ${CODEGEN_BINARY_DIR}
   COMMAND make -C ${CODEGEN_BINARY_DIR}
   COMMENT "Generate C++ code with cog python tool"
   VERBATIM
)

set_target_properties(main PROPERTIES
   COMPILE_OPTIONS "-I${CODEGEN_BINARY_DIR}"
   LINK_OPTIONS ""
   RUNTIME_OUTPUT_DIRECTORY ""
)
```

* `internal`文件夹用于代码生成，在执行前被拷贝到目标工作目录`CMAKE_CURRENT_BINARY_DIR`下
* 由于当前目标依赖生成的源文件，所以在编译的时候要加入头文件目录，以防止自动生成了依赖的头文件在编译时无法找到
* 编译命令
    * 源文件自动生成命令
    ```bash
    [ 25%] Generate C++ code with cog python tool
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src && /usr/bin/cmake -E copy_directory /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/src/internal /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src/internal
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src && make -C /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src/internal
    make[4]: Entering directory '/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src/internal'
    python3 -m cogapp -d -s " //cog generated" -o animal.cpp animal.cpp.cog
    make[4]: Leaving directory '/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src/internal'
    ```
    * 编译链接命令
    ```bash
    [ 50%] Building CXX object src/CMakeFiles/main.dir/main.cpp.o
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src && /usr/bin/c++   -g -I/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src/internal -std=c++20 -MD -MT src/CMakeFiles/main.dir/main.cpp.o -MF CMakeFiles/main.dir/main.cpp.o.d -o CMakeFiles/main.dir/main.cpp.o -c /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/src/main.cpp
    [ 75%] Building CXX object src/CMakeFiles/main.dir/internal/animal.cpp.o
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src && /usr/bin/c++   -g -I/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src/internal -std=c++20 -MD -MT src/CMakeFiles/main.dir/internal/animal.cpp.o -MF CMakeFiles/main.dir/internal/animal.cpp.o.d -o CMakeFiles/main.dir/internal/animal.cpp.o -c /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src/internal/animal.cpp
    [100%] Linking CXX executable main
    cd /home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build/src && /usr/bin/cmake -E cmake_link_script CMakeFiles/main.dir/link.txt --verbose=1
    /usr/bin/c++ -g CMakeFiles/main.dir/main.cpp.o CMakeFiles/main.dir/internal/animal.cpp.o -o main 
    make[3]: Leaving directory '/home/yuxiangw/GitHub/learning_book/docs/demos/cmake/code/custom/build'
    ```

## 参考
* [cmake常见变量](https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/Useful-Variables)
* [gcc常见编译选项](https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html)