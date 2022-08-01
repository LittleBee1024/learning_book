all: main

main:
	gcc -Og -o main main.c mstore.c

s:
	gcc -Og -S mstore.c

o:
	gcc -Og -c mstore.c
	objdump -d mstore.o

clean:
	rm -rf *.s *.o main
