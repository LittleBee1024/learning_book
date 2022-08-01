all: main

main:
	gcc -O0 -o main main.c mstore.c

s:
	gcc -O0 -S mstore.c

# objdump -d mstore.o
o:
	gcc -O0 -c mstore.c

clean:
	rm -rf *.s *.o main
