CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=-pthread

SRCS=main.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean main debug

all: clean main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

clean:
	@rm -rf main *.o

# Check semaphore value:
#  ipcs -s -i <semid>
#  cat /proc/sysvipc/sem
debug:
	gdb -q ./main \
	-ex "start" \
	-ex "shell ipcs -s"
