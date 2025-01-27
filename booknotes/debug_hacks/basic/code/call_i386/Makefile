# Install gcc multilib package (path /usr/lib32) for link error: cannot find -lgcc
#   sudo apt install gcc-multilib
CC=gcc
CFLAGS=-g3 -O0 -fno-inline -m32
LDFLAGS=-m32

SRCS=main.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean main

all: clean main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

clean:
	@rm -rf main *.o core

run:
	@./main

debug:
	gdb -q ./main \
	-ex 'b *func' \
	-ex 'run' \
	-ex 'p *(int*)($$esp+4)' \
	-ex 'p *(long*)($$esp+8)' \
	-ex 'p *(short*)($$esp+12)' \
	-ex 'p *(char*)($$esp+16)' \
	-ex 'p *(long long*)($$esp+20)' \
	-ex 'printf "%.2e\n",*(float*)($$esp+28)' \
	-ex 'printf "%.3e\n",*(double*)($$esp+32)' \
	-ex 'p/x *(int*)($$esp+40)' \
	-ex 'p/x *(int*)($$esp+44)' \
	-ex 'p/x *(int*)($$esp+48)' \
	-ex 'n' \
	-ex 'bt'
