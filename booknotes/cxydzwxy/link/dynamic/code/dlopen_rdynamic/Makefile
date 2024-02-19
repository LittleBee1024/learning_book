CC=gcc
CFLAGS=-g3 -O0 -fno-inline

.PHONY: clean main main2

all: clean static shared main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

main: main.o
	$(CC) $^ -o $@ -L. -lstatic -ldl -rdynamic

main2:
	$(CC) main.o -o main -L. -lstatic -ldl

static: static.o
	ar rcs libstatic.a $^

shared: shared.o
	$(CC) -fPIC -shared $^ -o lib$@.so -L. -lstatic

clean:
	@rm -rf main *.o core *.so *.a

debug:
	readelf --dyn-syms ./main
