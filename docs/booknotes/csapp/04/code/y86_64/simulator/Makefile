CC=g++
CFLAGS=-Wall -g

TARGET=sim
BUILD_DIR=build

.PHONY: all $(TARGET) test

all: clean $(TARGET)

clean:
	@rm -rf $(BUILD_DIR)

# Source code in current folder
SRCS=state.cpp storage.cpp sim_interface.cpp sim_seq.cpp sim_yis.cpp pipe_regs.cpp sim_pipe.cpp main.cpp
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

# Use HCL tool to generate source code
HCL=../hcl/build/hcl
$(BUILD_DIR)/seq.o: seq.hcl
	@echo "Building the seq.hcl of SEQ"
	@mkdir -p '$(@D)'
	$(HCL) seq.hcl -o $(BUILD_DIR)/seq.cpp
	$(CC) $(CFLAGS) -I.. -c $(BUILD_DIR)/seq.cpp -o $@

$(BUILD_DIR)/pipe.o: pipe.hcl
	@echo "Building the seq.hcl of PIPELINE"
	@mkdir -p '$(@D)'
	$(HCL) pipe.hcl -o $(BUILD_DIR)/pipe.cpp
	$(CC) $(CFLAGS) -I.. -c $(BUILD_DIR)/pipe.cpp -o $@

# Source code in dependent folder
DEP_COMMON_DIR=../_common
DEP_COMMON_SRCS=output.cpp isa.cpp
DEP_COMMON_OBJS=$(DEP_COMMON_SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(DEP_COMMON_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

# Final target
$(TARGET): $(DEP_COMMON_OBJS) $(BUILD_DIR)/seq.o $(BUILD_DIR)/pipe.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

# Test and debug
YAS=../assembler/build/yas
Y86_64_CODE=./test/asum.yo
%.yo: %.ys
	$(YAS) $< -o $@

test: $(Y86_64_CODE)
	@rm -rf test/*.log
	$(BUILD_DIR)/sim -y $(Y86_64_CODE) > test/yis.log
	$(BUILD_DIR)/sim -s $(Y86_64_CODE) > test/seq.log
	$(BUILD_DIR)/sim -p $(Y86_64_CODE) > test/pipe.log
	@cd test && diff yis.log yis.log.golden && \
               diff seq.log seq.log.golden && \
               diff pipe.log pipe.log.golden && \
               echo 'TEST_PASSED'

run_yis: $(Y86_64_CODE)
	$(BUILD_DIR)/sim -y $(Y86_64_CODE)

run_seq: $(Y86_64_CODE)
	$(BUILD_DIR)/sim -s $(Y86_64_CODE)

run_pipe: $(Y86_64_CODE)
	$(BUILD_DIR)/sim -p $(Y86_64_CODE)

debug_yis:
	gdb -q $(BUILD_DIR)/sim \
	-ex "start -y $(Y86_64_CODE)"

debug_seq:
	gdb -q $(BUILD_DIR)/sim \
	-ex "start -s $(Y86_64_CODE)"

debug_pipe:
	gdb -q $(BUILD_DIR)/sim \
	-ex "start -p $(Y86_64_CODE)"
