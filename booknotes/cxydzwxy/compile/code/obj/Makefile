.PHONY: clean compile

all: clean compile

clean:
	@rm -rf main *.o

compile:
	gcc -c main.c -o main.o

debug:
	objdump -x main.o
