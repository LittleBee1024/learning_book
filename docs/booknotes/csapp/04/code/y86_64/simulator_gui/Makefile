CC=g++
CFLAGS=-Wall -g

TARGET=gsim
BUILD_DIR=build

.PHONY: all $(TARGET) test

all: clean $(TARGET)

clean:
	@rm -rf $(BUILD_DIR) *.o *.log

# Source code in current folder
SRCS=main.cpp c_api_sim.cpp singleton.cpp
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

TCL_INCLUDE=/usr/include/tcl
TCL_LINK=-L/usr/lib/x86_64-linux-gnu -ltcl -ltk

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -I$(TCL_INCLUDE) -c $< -o $@

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
	$(CC) $(CFLAGS) $^ $(TCL_LINK) -o $(BUILD_DIR)/$@

# Test and debug
Y86_64_CODE=./test/asum.yo
TCL_CMD=./sim.tcl
run:
	$(BUILD_DIR)/$(TARGET) -c $(Y86_64_CODE) -t $(TCL_CMD)

