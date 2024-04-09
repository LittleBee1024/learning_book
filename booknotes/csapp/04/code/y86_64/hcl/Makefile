CC=g++
CFLAGS=-Wall -g
YACC=bison
LEX=flex

TARGET=hcl
BUILD_DIR=build

all: clean $(TARGET)

clean:
	@rm -rf *.o main $(BUILD_DIR)

# Source code in current folder
SRCS=outgen.cpp hcl.cpp main.cpp
SRC_OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

YACC_GEN=$(BUILD_DIR)/hcl_yacc_gen.cpp
YACC_OBJ=$(YACC_GEN:%.cpp=%.o)

$(YACC_OBJ): hcl.y
	@mkdir -p '$(@D)'
	$(YACC) -p hcl_ -d -o $(YACC_GEN) $<
	$(CC) $(CFLAGS) -I.. -c $(YACC_GEN) -o $@

LEX_GEN=$(BUILD_DIR)/hcl_lex_gen.cpp
LEX_OBJ=$(LEX_GEN:%.cpp=%.o)

$(LEX_OBJ): hcl.lex
	@mkdir -p '$(@D)'
	$(LEX) -P hcl_ -o $(LEX_GEN) $<
	$(CC) $(CFLAGS) -I.. -c $(LEX_GEN) -o $@

# Source code in dependent folder
DEP_COMMON_DIR=../_common
DEP_COMMON_SRCS=input.cpp output.cpp
DEP_COMMON_OBJS=$(DEP_COMMON_SRCS:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(DEP_COMMON_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I.. -c $< -o $@

# Final target
$(TARGET): $(DEP_COMMON_OBJS) $(YACC_OBJ) $(LEX_OBJ) $(SRC_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

# Test and debug
Y86_64_HCL=../test_hcl/mux4.hcl
run:
	$(BUILD_DIR)/$(TARGET) $(Y86_64_HCL)

debug:
	gdb -q $(BUILD_DIR)/$(TARGET) \
	-ex "start $(Y86_64_HCL)"

