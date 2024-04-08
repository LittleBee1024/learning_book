all:
	gcc -g -c main.c
	objdump -d -M intel -S main.o > main.s
	gcc -fverbose-asm -S main.c -o main2.s

clean:
	rm -rf *.o *.s
