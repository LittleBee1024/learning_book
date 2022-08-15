all: clean main

clean:
	rm -rf *_gen.cpp *_gen.hpp *.o main

yacc:
	bison -d -o calc_yacc_gen.cpp calc.y

lex: yacc
	flex -o calc_lex_gen.cpp calc.lex

main: clean lex
	g++ -g -Og -Wall calc_yacc_gen.cpp calc_lex_gen.cpp calc.cpp main.cpp -o main

run:
	./main test.txt

debug:
	gdb -q ./main \
	-ex "start test.txt"
