.PHONY: clean main

all: clean main

# 和main1效果一样，https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html#index-static
main: clean
	gcc -c -fno-builtin main.c
	gcc -static -o main main.o -Wl,--verbose

# 指定"ld -verbose"链接脚本编译，__executable_start指定为0x400000
# PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x400000)); . = SEGMENT_START("text-segment", 0x400000) + SIZEOF_HEADERS;
main1: clean
	gcc -c -fno-builtin main.c
	gcc -T main.lds -o main main.o -Wl,--verbose

# 默认编译方式，使用地址空间布局随机化，__executable_start指定为零
# PROVIDE (__executable_start = SEGMENT_START("text-segment", 0)); . = SEGMENT_START("text-segment", 0) + SIZEOF_HEADERS;
main2: clean
	gcc -c -fno-builtin main.c
	gcc -o main main.o -Wl,--verbose

# 关闭地址空间布局随机化ASLR
#	方法1: "setarch -R"
#	方法2: "echo 0 | sudo tee /proc/sys/kernel/randomize_va_space"
run:
	echo 0 | sudo tee /proc/sys/kernel/randomize_va_space && ./main

clean:
	@rm -rf main *.o core *.so

debug:
	gdb -q ./main \
	-ex 'start' \
	-ex 'i proc mapping' \
	-ex 'c'
