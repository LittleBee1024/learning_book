all: main

.PHONY: main clean

main: clean
	gcc -m32 -c -fno-builtin main.c
	ld -m elf_i386 -static -T main.lds -o main main.o

main2: clean
	gcc -m32 -c -fno-builtin main.c
	ld -m elf_i386 -static -e nomain -o main main.o

clean:
	@rm -rf main *.o

debug:
	gdb -q ./main \
	-ex 'b nomain' \
	-ex 'run' \
	-ex 'i proc mapping'
