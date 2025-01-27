CC=g++
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=

SRCS=main.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean main debug

all: clean main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

run:
	@./main in.txt out.log && diff in.txt out.log && echo "PASS"

clean:
	@rm -rf main *.o

debug:
	gdb -q ./main \
	-ex "start" \
	-ex "bt"
