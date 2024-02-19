CC=g++
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=

SRCS=main.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: clean main

all: clean main

%.o:%.cpp
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Start server, then test with nc
#	Client1: nc -q 1 127.0.0.1 1234
#	Client2: nc -q 1 127.0.0.1 1234
run:
	./main 127.0.0.1 1234

clean:
	@rm -rf main *.o

debug:
	gdb -q ./main \
	-ex 'start 127.0.0.1 1234'
