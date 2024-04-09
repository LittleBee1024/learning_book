CC = g++
CFLAGS  = -g
TARGET = main


all: $(TARGET)

.PHONY: clean run

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -c $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o

clean:
	/bin/sh -c 'rm -rf $(TARGET) $(TARGET).o'

debug:
	gdb -q ./main \
	-ex 'b main' \
	-ex 'run' \
	-ex 'set var gFlags=5' \
	-ex 'watch sum if Logme(sum)>0' \
	-ex 'c'
