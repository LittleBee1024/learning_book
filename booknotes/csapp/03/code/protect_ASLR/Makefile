all:
	gcc main.c -o main

clean:
	rm -rf main *.o

run:
	@setarch `uname -m` -R ./main
