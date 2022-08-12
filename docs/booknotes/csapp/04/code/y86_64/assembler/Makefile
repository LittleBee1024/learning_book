CC=g++
CFLAGS=-Wall -g
# http://dinosaur.compilertools.net/flex/manpage.html
LEX=flex

BUILD_DIR=build

LEX_GEN=yas_gen.cpp
SRCS=yas_instr.cpp yas.cpp input.cpp output.cpp main.cpp
OBJS=$(LEX_GEN:%.cpp=$(BUILD_DIR)/%.o) $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

DEP_DIR=../_isa
DEP_SRCS=instruction.cpp register.cpp
DEP_OBJS=$(DEP_SRCS:%.cpp=$(BUILD_DIR)/%.o)

TARGET=yas

all: clean $(TARGET)

clean:
	@rm -rf *.o main $(LEX_GEN) $(BUILD_DIR)

$(LEX_GEN): yas.lex
	$(LEX) -P yas -o $@ $<

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(DEP_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_DIR) -c $< -o $@

$(TARGET): $(OBJS) $(DEP_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

Y86_64_CODE=../test/prog1.ys
debug:
	gdb -q $(BUILD_DIR)/$(TARGET) \
	-ex "start $(Y86_64_CODE)"
