CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=

.PHONY: clean main lib

all: clean main

# "-Xlinker -rpath ./" sets so search folder, to find a1.so and a2.so
# can also use LD_LIBRARY_PATH env to specify search folder
main: lib
	$(CC) $(CFLAGS) main.c b1.so b2.so -o main -Xlinker -rpath ./

lib:
	$(CC) -fPIC -shared a1.c -o a1.so
	$(CC) -fPIC -shared a2.c -o a2.so
	$(CC) -fPIC -shared b1.c a1.so -o b1.so
	$(CC) -fPIC -shared b2.c a2.so -o b2.so

clean:
	@rm -rf main *.o core *.so

run:
	env LD_LIBRARY_PATH=. ./main

debug:
	env LD_LIBRARY_PATH=. gdb -q ./main \
	-ex 'start' \
	-ex 'i proc mapping'
