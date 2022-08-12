CC=g++
CFLAGS=-Wall -g

BUILD_DIR=build

SRCS=input.cpp output.cpp
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

all: clean compile

clean:
	@rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -c $< -o $@

compile: $(OBJS)
