CC=g++
CFLAGS=
LDFLAGS=

SRCS=main.c mm_symbol.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean main debug

all: main sym

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

sym:
	g++ -g -c -fPIC -o mm_symbol2.o mm_symbol.c

clean:
	@rm -rf main *.o *.i *.s

debug:
	gdb -q ./main \
	-ex 'run' \
	-ex 'ptype FreeBlock' \
	-ex 'p g_blocks' \
	-ex 'add-symbol-file mm_symbol2.o' \
	-ex 'ptype FreeBlock' \
	-ex 'p g_blocks'

