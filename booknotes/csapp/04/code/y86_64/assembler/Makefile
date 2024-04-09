CC=g++
CFLAGS=-Wall -g
LEX=flex

TARGET=yas
BUILD_DIR=build

all: clean $(TARGET)

clean:
	@rm -rf *.o main $(BUILD_DIR)

# Source code in current folder
SRCS=yas_instr.cpp yas.cpp main.cpp
SRC_OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

LEX_GEN=$(BUILD_DIR)/yas_gen.cpp
LEX_OBJ=$(LEX_GEN:%.cpp=%.o)

$(LEX_OBJ): yas.lex
	@mkdir -p '$(@D)'
	$(LEX) -P yas_ -o $(LEX_GEN) $<
	$(CC) $(CFLAGS) -I.. -c $(LEX_GEN) -o $@

# Source code in dependent folder
DEP_COMMON_DIR=../_common
DEP_COMMON_SRCS=input.cpp output.cpp isa.cpp
DEP_COMMON_OBJS=$(DEP_COMMON_SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(DEP_COMMON_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

# Final target
$(TARGET): $(DEP_COMMON_OBJS) $(LEX_OBJ) $(SRC_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

# Test and debug
Y86_64_CODE=../test_yas/prog1.ys
run:
	$(BUILD_DIR)/$(TARGET) $(Y86_64_CODE)

debug:
	gdb -q $(BUILD_DIR)/$(TARGET) \
	-ex "start $(Y86_64_CODE)"
