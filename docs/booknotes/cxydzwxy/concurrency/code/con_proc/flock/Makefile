CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=-pthread

SRCS=main.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean main debug

all: main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
	@rm -rf main *.o *.lock

debug:
	gdb -q ./main \
	-ex "set follow-fork-mode child" \
	-ex "start" \
	-ex "bt"
