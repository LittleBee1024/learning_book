CC=g++
CFLAGS=-Wall -g

BUILD_DIR=build

all: clean compile

clean:
	@rm -rf $(BUILD_DIR) *.o

# Source code in dependent folder
SRCS=input.cpp output.cpp isa.cpp
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

compile: $(OBJS)
