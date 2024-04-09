all: main

.PHONY: main clean

main: clean
	gcc -c -fno-builtin main.c
	ld -static -T main.lds -o main main.o

main2: clean
	gcc -c -fno-builtin main.c
	ld -static -e nomain -o main main.o

clean:
	@rm -rf main *.o

debug:
	gdb -q ./main \
	-ex 'b nomain' \
	-ex 'run' \
	-ex 'i proc mapping'
