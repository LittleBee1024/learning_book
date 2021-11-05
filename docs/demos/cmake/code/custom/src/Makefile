all: build

# require python package "cog"
gen:
	cd internal && python3 -m cogapp -d -s " //cog generated" -o animal.cpp animal.cpp.cog

build: clean gen
	g++ -c main.cpp -o main.o -I./internal
	g++ -c ./internal/animal.cpp -o animal.o -I./internal
	g++ -std=c++20 main.o animal.o -o main

clean:
	@rm -rf ./internal/animal.cpp main *.o

.PHYON: all gen build clean
