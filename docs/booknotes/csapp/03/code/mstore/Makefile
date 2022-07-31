all: main

main:
	gcc -Og -o main main.c mstore.c

s:
	gcc -Og -S mstore.c

# objdump -d mstore.o
o:
	gcc -Og -c mstore.c

clean:
	rm -rf *.s *.o main
