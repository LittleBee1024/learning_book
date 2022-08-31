CC=g++
CFLAGS=-Wall -g

TARGET=gsim
BUILD_DIR=build

.PHONY: all $(TARGET) test

all: clean $(TARGET)

clean:
	@rm -rf $(BUILD_DIR)

# Source code in current folder
SRCS=c_api_sim.cpp main.cpp
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

# Source code in dependent folder
DEP_SIM_DIR=../simulator
DEP_SIM_SRCS=sim_yis.cpp sim_interface.cpp storage.cpp state.cpp
DEP_SIM_OBJS=$(DEP_SIM_SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(DEP_SIM_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

DEP_COMMON_DIR=../_common
DEP_COMMON_SRCS=output.cpp isa.cpp
DEP_COMMON_OBJS=$(DEP_COMMON_SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(DEP_COMMON_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

# Final target
$(TARGET): $(OBJS) $(DEP_SIM_OBJS) $(DEP_COMMON_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

# Test and debug
Y86_64_CODE=./test/asum.yo
