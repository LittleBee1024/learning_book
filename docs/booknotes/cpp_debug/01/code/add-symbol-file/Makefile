CC=g++

SRCS=main.c

.PHONY: clean main debug

all: clean sym sym_d main

sym:
	$(CC) -fPIC -shared -c mm_symbol.c -o mm_symbol.o
	$(CC) -fPIC -shared mm_symbol.o -o libsym.so

sym_d:
	$(CC) -fPIC -shared -g -c mm_symbol.c -o mm_symbol_d.o
	$(CC) -fPIC -shared mm_symbol_d.o -o libsym_d.so

main:
	$(CC) -c main.c -o main.o
	$(CC) main.o -L. -lsym -o main -Wl,-rpath=.

clean:
	@rm -rf main *.o *.i *.s *.so

debug:
	gdb -q ./main \
	-ex 'run' \
	-ex 'ptype FreeBlock' \
	-ex 'p g_blocks' \
	-ex 'add-symbol-file mm_symbol_d.o' \
	-ex 'ptype FreeBlock' \
	-ex 'p g_blocks'

