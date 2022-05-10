CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=-lrt

SRCS=main.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean main debug

all: clean main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

clean:
	@rm -rf main *.o

# ls /dev/shm
debug:
	gdb -q ./main \
	-ex "set follow-fork-mode child" \
	-ex "start" \
	-ex "bt"
