CC=g++
CFLAGS=-Wall -g
# http://dinosaur.compilertools.net/flex/manpage.html
LEX=flex

TARGET=yas

all: clean $(TARGET)

BUILD_DIR=build

DEP_COMMON_DIR=../_common
DEP_COMMON_SRCS=input.cpp output.cpp
DEP_COMMON_OBJS=$(DEP_COMMON_SRCS:%.cpp=$(BUILD_DIR)/%.o)

DEP_ISA_DIR=../_isa
DEP_ISA_SRCS=instruction.cpp register.cpp
DEP_ISA_OBJS=$(DEP_ISA_SRCS:%.cpp=$(BUILD_DIR)/%.o)

LEX_GEN=$(BUILD_DIR)/yas_gen.cpp
LEX_OBJ=$(LEX_GEN:%.cpp=%.o)

SRCS=yas_instr.cpp yas.cpp main.cpp
SRC_OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

clean:
	@rm -rf *.o main $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(DEP_COMMON_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_COMMON_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(DEP_ISA_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_ISA_DIR) -c $< -o $@

$(LEX_OBJ): yas.lex
	@mkdir -p '$(@D)'
	$(LEX) -P yas_ -o $(LEX_GEN) $<
	$(CC) $(CFLAGS) -I. -I$(DEP_COMMON_DIR) -I$(DEP_ISA_DIR) -c $(LEX_GEN) -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_ISA_DIR) -I$(DEP_COMMON_DIR) -c $< -o $@

$(TARGET): $(DEP_COMMON_OBJS) $(DEP_ISA_OBJS) $(LEX_OBJ) $(SRC_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

Y86_64_CODE=../test_yas/prog1.ys
run:
	$(BUILD_DIR)/$(TARGET) $(Y86_64_CODE)

debug:
	gdb -q $(BUILD_DIR)/$(TARGET) \
	-ex "start $(Y86_64_CODE)"
