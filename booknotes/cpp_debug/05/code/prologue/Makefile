all:
	gcc -g -c main.c
	objdump -d -S main.o > main.s
	gcc -fverbose-asm -S main.c -o main2.s
	gcc main.c -g -o main

clean:
	rm -rf *.o *.s

debug:
	gdb -q main \
	-ex 'b *&main' \
	-ex 'b *&f' \
	-ex 'run' \
	-ex 'layout asm' \
	-ex 'si' \
	-ex 'info register rsp'