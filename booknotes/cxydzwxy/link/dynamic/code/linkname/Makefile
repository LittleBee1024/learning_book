CC=gcc
CFLAGS=-g3 -O0 -fno-inline

SRCS=main.c
OBJS=$(SRCS:.c=.o)

SRCS2=lib.c
OBJS2=$(SRCS2:.c=.o)

.PHONY: clean main main2 main3 main_static foo foo_static

all: main

main: clean foo $(OBJS)
	$(CC) $(OBJS) -L. -lfoo -o main

main2: clean foo $(OBJS)
	$(CC) $(OBJS) -L. -l:libfoo.so -o main

main3: clean foo $(OBJS)
	$(CC) $(OBJS) $(PWD)/libfoo.so -o main

main_static: clean foo_static $(OBJS)
	$(CC) $(OBJS) -L. -Wl,-static -lfoo -Wl,-Bdynamic -o main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

$(OBJS2) : %.o: %.c
	$(CC) -fPIC -shared $(CFLAGS) -c $<

foo: $(OBJS2)
	$(CC) -fPIC -shared -Wl,-soname,libfoo.so $(OBJS2) -o libfoo.so

foo_static: $(OBJS2)
	ar rcs libfoo.a $(OBJS2)

clean:
	@rm -rf main *.o core *.so *.a

run:
	@env LD_LIBRARY_PATH=. ./main

run_static:
	./main
