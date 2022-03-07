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
	$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
	@rm -rf main *.o