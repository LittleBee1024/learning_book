compile:
	g++ -g main.cpp -o main

asan:
	g++ -fsanitize=address -g main.cpp -o main

check:
	env MALLOC_CHECK_=0 ./main

clean:
	rm -rf *.log *.s *.o
