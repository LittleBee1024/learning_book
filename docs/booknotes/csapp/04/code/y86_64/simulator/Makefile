CC=g++
CFLAGS=-Wall -g

TARGET=yis

BUILD_DIR=build

SRCS=state.cpp storage.cpp yis.cpp main.cpp
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

DEP_COMMON_DIR=../_common
DEP_COMMON_SRCS=output.cpp isa.cpp
DEP_COMMON_OBJS=$(DEP_COMMON_SRCS:%.cpp=$(BUILD_DIR)/%.o)

all: clean $(TARGET)

clean:
	@rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_COMMON_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(DEP_COMMON_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_COMMON_DIR) -c $< -o $@

$(TARGET): $(DEP_COMMON_OBJS) $(OBJS)
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

