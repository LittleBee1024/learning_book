o:
	gcc -Og -c mstore.c
	objdump -d mstore.o

s:
	gcc -Og -S mstore.c

main:
	gcc -Og -o main main.c mstore.c

clean:
	rm -rf *.s *.o main
