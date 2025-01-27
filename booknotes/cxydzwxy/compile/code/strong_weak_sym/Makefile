CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=

SRCS=main.c weak.c
OBJS=$(SRCS:.c=.o)

SRCS2=main.c weak.c strong.c
OBJS2=$(SRCS2:.c=.o)

.PHONY: clean program1 program2

all: clean program1 program2

%.o:%.c
	$(CC) $(CFLAGS) -c $<

program1: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

program2: $(OBJS2)
	$(CC) $(LDFLAGS) $(OBJS2) -o $@

run:
	./program1
	./program2

clean:
	@rm -rf program1 program2 *.o