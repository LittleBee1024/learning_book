all: main

.PHONY: main clean

main: clean
	g++ -c foo.cpp obj1.cpp obj2.cpp main.cpp
	g++ foo.o obj1.o obj2.o main.o -o main

clean:
	@rm -rf main *.o

