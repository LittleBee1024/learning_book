CC=g++
CFLAGS=-Wall -g

TARGET=yis

all: clean $(TARGET)

BUILD_DIR=build

DEP_ISA_DIR=../_isa
DEP_ISA_SRCS=isa.cpp
DEP_ISA_OBJS=$(DEP_ISA_SRCS:%.cpp=$(BUILD_DIR)/%.o)

SRCS=main.cpp
SRC_OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

clean:
	@rm -rf *.o main $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(DEP_ISA_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_ISA_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_ISA_DIR) -c $< -o $@

$(TARGET): $(DEP_ISA_OBJS) $(SRC_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

YAS=../assembler/build/yas
Y86_64_CODE=../test_yas/asum.yo

%.yo: %.ys
	$(YAS) $< -o $@

run: $(Y86_64_CODE)
	$(BUILD_DIR)/$(TARGET) $(Y86_64_CODE)

debug:
	gdb -q $(BUILD_DIR)/$(TARGET) \
	-ex "start $(Y86_64_CODE)"
