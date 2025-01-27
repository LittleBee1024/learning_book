CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=-L. -lfoo

SRCS=main.c
OBJS=$(SRCS:.c=.o)

SRCS2=foo.c
OBJS2=$(SRCS2:.c=.o)

.PHONY: clean main foo

all: clean main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: foo $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJS2): %.o: %.c
	$(CC) -fPIC -shared $(CFLAGS) -c $<

foo: $(OBJS2)
	$(CC) -fPIC -shared $(OBJS2) -o lib$@.so

clean:
	@rm -rf main *.o core *.so

run:
	@env LD_LIBRARY_PATH=. ./main

debug:
	env LD_LIBRARY_PATH=. gdb -q ./main \
	-ex 'start' 
