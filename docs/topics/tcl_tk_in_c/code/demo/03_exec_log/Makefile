all: clean main

main:
	gcc -I/usr/include/tcl -c main.c -o main.o
	gcc main.o -L/usr/lib/x86_64-linux-gnu -ltcl -ltk -o main

run:
	./main main.tcl

clean:
	@rm -rf *.o main
