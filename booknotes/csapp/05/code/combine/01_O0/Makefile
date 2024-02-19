CC=g++
CFLAGS=-g3 -fno-inline -O0
LDFLAGS=

SRCS=main.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: clean main debug

all: clean main

%.o:%.cpp
	$(CC) $(CFLAGS) -c $<

main: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
	@rm -rf main *.o *.i *.s *.out *.log

debug:
	gdb -q ./main -ex "start"

profile: clean
	g++ -Og -pg main.cpp -o main
	./main
	gprof main | tee prof.log
