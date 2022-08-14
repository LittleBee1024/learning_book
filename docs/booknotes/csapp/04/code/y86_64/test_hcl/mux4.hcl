## Simple example of an HCL file.
## This file can be converted to C using hcl2c, and then compiled.

## In this example, we will generate the MUX4 circuit shown in
## Section 4.2.4. It consists of a control block that generates
## bit-level signals s1 and s0 from the input signal code,
## and then uses these signals to control a 4-way multiplexor
## with data inputs A, B, C, and D.

## This code is embedded in a C program that reads5
## the values of code, A, B, C, and D from the command line
## and then prints the circuit output

## Information that is inserted verbatim into the C file
quote '#include <stdio.h>'
quote '#include <stdlib.h>'
quote 'int code_val, s0_val, s1_val;'
quote 'char **data_names;'

## Declarations of signals used in the HCL description and
## the corresponding C expressions.
boolsig s0 's0_val'
boolsig s1 's1_val'
wordsig code 'code_val'
wordsig A 'atoi(data_names[0])'
wordsig B 'atoi(data_names[1])'
wordsig C 'atoi(data_names[2])'
wordsig D 'atoi(data_names[3])'

## HCL descriptions of the logic blocks
bool s1 = code in { 2, 3 };

bool s0 = code in { 1, 3 };

word Out4 = [
   !s1 && !s0 : A; # 00
   !s1 : B; # 01
   !s0 : C; # 10
   1 : D; # 11
];

## More information inserted verbatim into the C code to
## compute the values and print the output
quote 'int main(int argc, char *argv[]) {'
quote '    if (argc < 6) {'
quote '        printf("Please input five arguments: code, A, B, C and D, such as: ./mux4 0 1 2 3 4\n");'
quote '        return -1;'
quote '    }'
quote '    data_names = argv+2;'
quote '    code_val = atoi(argv[1]);'
quote '    s1_val = gen_s1();'
quote '    s0_val = gen_s0();'
quote '    printf("Out = %lld\n", gen_Out4());'
quote '    return 0;'
quote '}'
