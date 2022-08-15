all: clean main

clean:
	rm -rf *_gen.cpp *.o main

lex:
	flex -P c_wc -o c_wc_gen.cpp c_wc.lex

main: lex
	g++ -Wall c_wc_gen.cpp lexer.cpp main.cpp -o main

run:
	./main test.c
