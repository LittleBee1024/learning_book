CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=

SRCS=sum.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean main

all: main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

clean:
	@rm -rf main *.o core

run:
	@./main 10

debug:
	gdb -q ./main \
	-ex "disas main" \
	-ex "disas sum_till_MAX"

debug_stack:
	gdb -q ./main \
	-ex 'b sum.c:18' \
	-ex 'run 10' \
	-ex 'c' \
	-ex 'bt' \
	-ex 'x/40xw $$sp'

# Disable core: ulimit -S -c 0
debug_core:
	-ulimit -S -c unlimited && ./main
	gdb -q main --core=core -ex 'info target' -ex 'p $$sp'

debug_fault:
	gdb -q ./main \
	-ex 'r' \
	-ex 'i proc mapping' \
	-ex 'p $$sp'
