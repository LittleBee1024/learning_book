main: clean
	gcc -Og -o main main.c mstore.c

o:
	gcc -Og -c mstore.c
	objdump -d mstore.o

s:
	gcc -Og -S mstore.c
	gcc -Og -S main.c

clean:
	rm -rf *.s *.o main

debug:
	gdb -q ./main \
	-ex 'start' \
	-ex 'layout asm'
