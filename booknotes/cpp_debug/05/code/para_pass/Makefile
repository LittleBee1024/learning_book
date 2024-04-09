all:
	g++ -g -c main.cpp
	objdump -d -S main.o > main.s
	g++ main.cpp -g -o main

clean:
	rm -rf *.o *.s main

debug:
	gdb -q main \
	-ex 'b *&Sum' \
	-ex 'run' \
	-ex 'info registers'