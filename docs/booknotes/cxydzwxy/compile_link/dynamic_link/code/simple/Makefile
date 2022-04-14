CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=-L. -lfoo

SRCS=program1.c
OBJS=$(SRCS:.c=.o)

SRCS2=program2.c
OBJS2=$(SRCS2:.c=.o)

SRCS3=lib.c
OBJS3=$(SRCS3:.c=.o)

.PHONY: clean foo program1 program2

all: clean foo program1 program2

%.o:%.c
	$(CC) $(CFLAGS) -c $<

program1: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

program2: $(OBJS2)
	$(CC) $(OBJS2) $(LDFLAGS) -o $@

$(OBJS3): %.o: %.c
	$(CC) -fPIC -shared $(CFLAGS) -c $<

foo: $(OBJS3)
	$(CC) -fPIC -shared $(OBJS3) -o lib$@.so

clean:
	@rm -rf main *.o core *.so program1 program2

run:
	@env LD_LIBRARY_PATH=. ./program1
	@env LD_LIBRARY_PATH=. ./program2

debug:
	env LD_LIBRARY_PATH=. gdb -q ./main \
	-ex 'start' 
