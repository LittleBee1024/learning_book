HCL_DIR=../hcl/build
HCL=$(HCL_DIR)/hcl

HCL_SRCS=mux4.hcl seq_std.hcl
HCL_C=$(HCL_SRCS:%.hcl=%.c)
HCL_V=$(HCL_SRCS:%.hcl=%.v)

all: clean to_c to_v

%.c: %.hcl
	$(HCL) -c $< -o $@

%.v: %.hcl
	$(HCL) -v $< -o $@

to_c: $(HCL_C)

to_v: $(HCL_V)

clean:
	rm -f *.c *.v *.o
