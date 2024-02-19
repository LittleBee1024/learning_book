.PHONY: all main clean

all: main

main: clean
	gcc -static main.c -o main -Wl,--verbose

run:
	readelf -S main
	readelf -l main

clean:
	@rm -rf main *.o

debug:
	gdb -q ./main \
	-ex 'start' \
	-ex 'i proc mapping' \
	-ex 'c'

