CC=g++
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=

SRC_1 = server.cpp
OBJS_1=$(SRC_1:.cpp=.o)

SRC_2 = client.cpp
OBJS_2=$(SRC_2:.cpp=.o)

.PHONY: clean server client

all: server client

%.o:%.cpp
	$(CC) $(CFLAGS) -c $<

server: $(OBJS_1)
	$(CC) $(LDFLAGS) $(OBJS_1) -o $@

client: $(OBJS_2)
	$(CC) $(LDFLAGS) $(OBJS_2) -o $@

clean:
	@rm -rf server client *.o


# Monitor addr/port
#  sudo tcpdump -nt -i lo port 12345
# Start server
# Start client
run_server:
	./server 127.0.0.1 1234

run_client:
	./client 127.0.0.1 1234

debug:
	gdb -q ./client \
	-ex "b client.cpp:30" \
	-ex "run 127.0.0.1 1234 "

