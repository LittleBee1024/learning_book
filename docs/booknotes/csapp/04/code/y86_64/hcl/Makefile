CC=g++
CFLAGS=-Wall -g
# http://dinosaur.compilertools.net/flex/manpage.html
YACC=bison
LEX=flex

TARGET=hcl

all: clean $(TARGET)

BUILD_DIR=build

SRCS=outgen.cpp hcl.cpp main.cpp
SRC_OBJS=$(SRCS:%.cpp=$(BUILD_DIR)/%.o)

DEP_COMMON_DIR=../_common
DEP_COMMON_SRCS=input.cpp output.cpp
DEP_COMMON_OBJS=$(DEP_COMMON_SRCS:%.cpp=$(BUILD_DIR)/%.o)

YACC_GEN=$(BUILD_DIR)/hcl_yacc_gen.cpp
YACC_OBJ=$(YACC_GEN:%.cpp=%.o)

LEX_GEN=$(BUILD_DIR)/hcl_lex_gen.cpp
LEX_OBJ=$(LEX_GEN:%.cpp=%.o)

clean:
	@rm -rf *.o main $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_COMMON_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(DEP_COMMON_DIR)/%.cpp
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -I$(DEP_COMMON_DIR) -c $< -o $@

$(YACC_OBJ): hcl.y
	@mkdir -p '$(@D)'
	$(YACC) -p hcl_ -d -o $(YACC_GEN) $<
	$(CC) $(CFLAGS) -I. -I$(DEP_COMMON_DIR) -c $(YACC_GEN) -o $@

$(LEX_OBJ): hcl.lex
	@mkdir -p '$(@D)'
	$(LEX) -P hcl_ -o $(LEX_GEN) $<
	$(CC) $(CFLAGS) -I. -I$(DEP_COMMON_DIR) -I$(BUILD_DIR) -c $(LEX_GEN) -o $@

$(TARGET): $(DEP_COMMON_OBJS) $(YACC_OBJ) $(LEX_OBJ) $(SRC_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@

Y86_64_HCL=../test_hcl/mux4.hcl
run:
	$(BUILD_DIR)/$(TARGET) $(Y86_64_HCL)

debug:
	gdb -q $(BUILD_DIR)/$(TARGET) \
	-ex "start $(Y86_64_HCL)"

