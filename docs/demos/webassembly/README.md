# WebAssembly

> 简单的“WebAssembly + C/Cpp”样例，所有样例可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/demos/webassembly/code)中找到

## WebAssembly with C
### [代码实例](./code/hello_c)
![web_c](./images/web_c.png)

* [C代码](./code/hello_c/api.c)
    * 通过`EMSCRIPTEN_KEEPALIVE`宏将C函数输出给JS代码
    * 支持字符串、整型和浮点数在C代码和JS代码之间传递
    ```c
    #include <stdio.h>

    #include <emscripten.h>

    EMSCRIPTEN_KEEPALIVE
    int PrintString(const char* str)
    {
        printf("[%s]: %s\n", __func__, str);
        return 1;
    }

    EMSCRIPTEN_KEEPALIVE
    const char* GetString()
    {
        static const char str[] = "String from C";
        return str;
    }

    EMSCRIPTEN_KEEPALIVE
    int AddInt(int a, int b)
    {
        int res = a + b;
        printf("[%s]: %d + %d = %d\n", __func__, a, b, res);
        return res;
    }

    EMSCRIPTEN_KEEPALIVE
    float DivideFloat(float a, float b)
    {
        float res = a / b;
        printf("[%s]: %f / %f = %f\n", __func__, a, b, res);
        return res;
    }
    ```

* [HTML代码](./code/hello_c/index.html)
    * JS中字符串可以创建在堆或者栈上，在堆上创建的字符串需要手动释放
    * 由于emcc默认不输出`malloc`和`free`，如果需要在堆上创建字符串，需要在编译时加入`-s "EXPORTED_FUNCTIONS=['_malloc', '_free']"`编译选项
    ```html
    <script>
        // Set up wasm module callback function to print info
        Module = {};
        Module.onRuntimeInitialized = function () {
            console.log(Module)
            // Print C function, malloc and free can be printed here
            // if they are exported intentionally when compiling
            console.log(Module['asm'])
        }
        // Set up button function
        function button() {
            // Pass string from JS to C
            strOnStack = allocateUTF8OnStack("Hello from JS stack");
            console.log(Module._PrintString(strOnStack));
            strOnHeap = allocateUTF8("Hello from JS heap");
            console.log(Module._PrintString(strOnHeap));
            Module._free(strOnHeap);

            // Get string from C to JS
            var str = UTF8ToString(Module._GetString());
            console.log(typeof (str));
            console.log(str);

            // Process Integer in C
            console.log(Module._AddInt(1, 2));

            // Process Float in C
            console.log(Module._DivideFloat(7, 2.5));
        }
    </script>
    ```

## WebAssembly with Cpp
### [代码实例](./code/hello_cpp)
![web_c](./images/web_cpp.png)

* [Cpp代码](./code/hello_cpp/api.cpp)
    * 为了防止C++的`name mangling`，所有的API都通过`extern "C"`修饰后，以C API的形式传给JS
    * 支持C++多态，STL库
    ```cpp
    EMSCRIPTEN_KEEPALIVE extern "C"
    int CppPoly()
    {
        std::unique_ptr<CPP::Animal> a_dog = std::make_unique<CPP::Dog>("wang wang");
        std::unique_ptr<CPP::Animal> a_bird = std::make_unique<CPP::Bird>("zha zha");

        a_dog->set_weight(10);
        a_bird->set_weight(2);

        a_dog->dump();
        a_bird->dump();

        return 1;
    }

    EMSCRIPTEN_KEEPALIVE extern "C"
    const char* ConvertString(const char* input)
    {
        static std::string s_str;
        s_str.clear();
        s_str.append(input);
        std::cout << "Append CPP to the string: " << s_str << std::endl;
        s_str.append(" CPP");
        std::cout << "Return: " << s_str << std::endl;
        return s_str.c_str();
    }
    ```
* [HTML代码](./code/hello_cpp/index.html)
    * 用法和上面的C代码例子类似，此处不再细述

## WebAssembly的内存模型
Emscripten提供了一个`ArrayBuffer`对象，用于C/C++和JS代码共享内存，默认内存大小为：16MB (2^24 = 16777216)。我们可以在浏览器终端通过`Module['asm']['memory'].buffer`得到`ArrayBuffer`对象。

对象 | TypedArray | 对应C数据类型
---- | ------- | -------
Module.HEAP8 | Int8Array | int8
Module.HEAP16 | Int16Array | int16
Module.HEAP32 | Int32Array | int32
Module.HEAPU8 | Uint8Array | uint8
Module.HEAPU16 | Uint16Array | uint16
Module.HEAPU32 | Uint32Array | uint32
Module.HEAPF32 | Float32Array | float
Module.HEAPF64 | Float64Array | double

在这16MB的内存空间中，Emscripten在其上面创建了8中view(如上表)，分别对应8中数据类型。例如，`Module['HEAP8'] = new Int8Array(Module['asm']['memory'].buffer)`就在`ArrayBuffer`对象上创建了一个`HEAP8`的view。下图中，所有View的空间内存空间是一样的，由编译器确保不同变量的地址不冲突。

![emcc_mem](./images/emcc_mem.png)

### [代码实例](./code/mem)
![web_mem](./images/web_mem.png)

* [Cpp代码](./code/mem/api.cpp)
    * C代码中的全局变量存在于`ArrayBuffer`对象，在JS中可通过对应的View加地址访问
    * 函数返回的指针，在JS代码中对应View上的地址
    ```c
    int g_int = 42;
    double g_double = 3.1415926;

    EMSCRIPTEN_KEEPALIVE
    int* GetIntPtr()
    {
        return &g_int;
    }

    EMSCRIPTEN_KEEPALIVE
    double* GetDoublePtr() {
        return &g_double;
    }

    EMSCRIPTEN_KEEPALIVE
    void PrintData() {
        printf("[%s]: g_int addr = %p, val = %d\n", __func__, &g_int, g_int);
        printf("[%s]: g_double addr = %p, val = %lf\n", __func__, &g_double, g_double);
    }
    ```
* [HTML代码](./code/mem/index.html)
    * 通过`Module.HEAP32[addr]`可以访问内存中的`int32`变量，其他类型方法雷同
    ```html
    <script>
        function button() {
            // HEAP32 heap
            var int_ptr = Module._GetIntPtr();
            // right shift 2 because sizeof(int) == 4(2^2)
            var int_value = Module.HEAP32[int_ptr >> 2];
            // Console output: Module.HEAP32[256] = 42
            console.log("Module.HEAP32[" +(int_ptr >> 2) + "] = " + int_value);

            // HEAPF64 heap
            var double_ptr = Module._GetDoublePtr();
            // right shift 2 because sizeof(double) == 8(2^3)
            var double_value = Module.HEAPF64[double_ptr >> 3];
            // Console output: Module.HEAPF64[201] = 3.1415926
            console.log("Module.HEAPF64[" +(double_ptr >> 3) + "] = " + double_value);

            // Modify the memory
            Module.HEAP32[int_ptr >> 2] = 13;
            Module.HEAPF64[double_ptr >> 3] = 123456.789
            // Console output: C{g_int:13} C{g_double:123456.789000}
            Module._PrintData()
        }
    </script>
    ```

## 参考
* [Emscripten编译选项](https://emscripten.org/docs/tools_reference/emcc.html)

