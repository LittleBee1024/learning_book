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
	-ex 'watch sum if times >= 2' \
	-ex 'c' \
	-ex 'info b'
