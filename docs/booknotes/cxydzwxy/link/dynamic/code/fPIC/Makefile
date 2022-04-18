CC=gcc
CFLAGS=-g3 -O0 -fno-inline
LDFLAGS=-L. -louter

SRCS=main.c
OBJS=$(SRCS:.c=.o)

SRCS2=lib_outer.c
OBJS2=$(SRCS2:.c=.o)

SRCS3=lib_inner.c
OBJS3=$(SRCS3:.c=.o)

.PHONY: clean inner outer main

all: clean inner outer main

%.o:%.c
	$(CC) $(CFLAGS) -c $<

# "-Xlinker -rpath ." or "-Wl,-rpath=."
#   to set lib search folder as current folder, to find a1.so and a2.so
# Or also use LD_LIBRARY_PATH env to specify search folder
main: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@ -Wl,-rpath=.

$(OBJS2) $(OBJS3): %.o: %.c
	$(CC) -fPIC -shared $(CFLAGS) -c $<

outer: $(OBJS2)
	$(CC) -fPIC -shared $(OBJS2) -L. -linner -o lib$@.so

inner: $(OBJS3)
	$(CC) -fPIC -shared $(OBJS3) -o lib$@.so

clean:
	@rm -rf main *.o core *.so

run:
	@env LD_LIBRARY_PATH=. ./main

debug:
	env LD_LIBRARY_PATH=. gdb -q ./main \
	-ex 'start' \
	-ex 'i proc mapping'
