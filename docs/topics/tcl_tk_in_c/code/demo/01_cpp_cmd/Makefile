all: clean main

main:
	g++ -c blob.cpp -o blob.o
	g++ -I/usr/include/tcl -c main.cpp -o main.o
	g++ blob.o main.o -L/usr/lib/x86_64-linux-gnu -ltcl -o main

run:
	./main cmd.tcl

clean:
	@rm -rf *.o main
