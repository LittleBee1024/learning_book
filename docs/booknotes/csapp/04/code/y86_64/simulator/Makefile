CC=g++
CFLAGS=-Wall -g

BUILD_DIR=build

SRCS=state.cpp storage.cpp sim_interface.cpp sim_seq.cpp sim_yis.cpp main.cpp
OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

DEP_COMMON_DIR=../_common
DEP_COMMON_SRCS=output.cpp isa.cpp
DEP_COMMON_OBJS=$(DEP_COMMON_SRCS:%.cpp=$(BUILD_DIR)/%.o)

all: clean sim

clean:
	@rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_COMMON_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(DEP_COMMON_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_COMMON_DIR) -c $< -o $@

HCL=../hcl/build/hcl
$(BUILD_DIR)/seq.o: seq.hcl
	@echo "Building the seq.hcl of SEQ+"
	@mkdir -p '$(@D)'
	$(HCL) seq.hcl -o $(BUILD_DIR)/seq.cpp
	$(CC) $(CFLAGS) -I. -I$(DEP_COMMON_DIR) -c $(BUILD_DIR)/seq.cpp -o $@


sim: $(DEP_COMMON_OBJS) $(BUILD_DIR)/seq.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

YAS=../assembler/build/yas
Y86_64_CODE=../test_yas/asum.yo

%.yo: %.ys
	$(YAS) $< -o $@

run_yis: $(Y86_64_CODE)
	$(BUILD_DIR)/sim -y $(Y86_64_CODE)

run_seq: $(Y86_64_CODE)
	$(BUILD_DIR)/sim -s $(Y86_64_CODE)

debug:
	gdb -q $(BUILD_DIR)/sim \
	-ex "start $(Y86_64_CODE)"

