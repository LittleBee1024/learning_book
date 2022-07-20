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
	@rm -rf main *.o

dwarf:
	readelf --debug-dump=info main > dwarf_info.log
