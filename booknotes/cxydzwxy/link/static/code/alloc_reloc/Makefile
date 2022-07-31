all: main

.PHONY: main clean

main: clean
	gcc -c a.c b.c
	ld a.o b.o -e main -o main

clean:
	@rm -rf main *.o

debug:
	objdump -h a.o; \
	objdump -h b.o; \
	objdump -h main;
