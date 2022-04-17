.PHONY: clean main

all: clean main

main:
	gcc main.c -o main
	readelf -S main

debug:
	gcc -g main.c -o main
	readelf -S main
	strip main
	readelf -S main

clean:
	@rm -rf main *.o