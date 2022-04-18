CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=-ldl

SRCS=main.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean main

all: clean main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

inner: $(OBJS3)
	$(CC) -fPIC -shared $(OBJS3) -o lib$@.so

clean:
	@rm -rf main *.o core *.so

run:
	./main libm.so.6
