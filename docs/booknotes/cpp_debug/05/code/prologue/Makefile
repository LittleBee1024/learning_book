all:
	gcc -g -c main.c
	objdump -d -S main.o > main.s
	gcc -fverbose-asm -S main.c -o main2.s
	gcc main.c -g -o main

clean:
	rm -rf *.o *.s
