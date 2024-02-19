asan:
	g++ -fsanitize=address -g main.cpp -o main
	./main

check:
	g++ main.cpp -o main
	env MALLOC_CHECK_=1 ./main

clean:
	rm -rf *.log *.s *.o
