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
	@rm -rf main *.o

run:
	@./main 10

debug:
	gdb -q ./main \
	-ex "disas main" \
	-ex "disas sum_till_MAX"

