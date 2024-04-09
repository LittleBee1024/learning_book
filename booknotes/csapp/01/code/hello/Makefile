CC=g++
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=

SRCS=main.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean main debug

all: main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
	@rm -rf main *.o *.i *.s

dump:
	hd main.c

i:
	gcc -E main.c -o main.i

s: i
	gcc -S main.i -o main.s

o: s
	gcc -c main.s -o main.o

l: o
	gcc main.o -o main
