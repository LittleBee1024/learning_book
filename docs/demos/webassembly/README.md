# WebAssembly

> 简单的“WebAssembly + C/Cpp”样例

## WebAssembly with C
* [代码示例](https://github.com/LittleBee1024/learning_book/tree/main/docs/demos/webassembly/code/hello_c)
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
* [代码示例](https://github.com/LittleBee1024/learning_book/tree/main/docs/demos/webassembly/code/hello_cpp)
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


## 参考
* [Emscripten编译选项](https://emscripten.org/docs/tools_reference/emcc.html)

