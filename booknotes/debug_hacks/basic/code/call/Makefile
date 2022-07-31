CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=

SRCS=main.c
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
	@./main

debug:
	gdb -q ./main \
	-ex 'b *func' \
	-ex 'run' \
	-ex 'i r' \
	-ex 'p $$xmm0.v4_float' \
	-ex 'p $$xmm1.v2_double' \
	-ex 'x/3xg $$rsp' \
	-ex 'printf "%.2f\n", *(float*)(*(unsigned long*)($$rsp+0x8))' \
	-ex 'p (char*)(*(unsigned long*)($$rsp+0x10))' \
	-ex 'n' \
	-ex 'bt'
