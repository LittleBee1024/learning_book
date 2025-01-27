.PHONY: clean preprocess compile assembly link link2

all: clean preprocess compile assembly link link2

clean:
	@rm -rf main *.o *.s *.i

preprocess:
	gcc -E main.c -o main.i

compile:
	gcc -S main.i -o main.s

assembly:
	gcc -c main.s -o main.o

link:
	ld -static /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/gcc/x86_64-linux-gnu/9/crtbeginT.o \
	-L/usr/lib/gcc/x86_64-linux-gnu/9 -L/usr/lib -L/lib main.o --start-group -lgcc -lgcc_eh -lc --end-group /usr/lib/gcc/x86_64-linux-gnu/9/crtend.o /usr/lib/x86_64-linux-gnu/crtn.o \
	-o main

link2:
	gcc main.o -o main

debug:
	gdb -q ./main \
	-ex 'start' 


