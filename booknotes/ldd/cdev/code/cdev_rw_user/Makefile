CC=gcc
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

clean:
	@rm -rf main *.o *.log

# load gmem0 module in "../cdev_rw"
test:
	./main
	tail -n 4 /var/log/kern.log
