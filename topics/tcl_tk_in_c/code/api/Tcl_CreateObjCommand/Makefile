all: clean main

main:
	gcc -I/usr/include/tcl -c main.c -o main.o
	gcc main.o -L/usr/lib/x86_64-linux-gnu -ltcl -o main

run:
	./main cmd.tcl

clean:
	@rm -rf *.o main
