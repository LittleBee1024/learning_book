all:
	g++ -g -S foo.cpp -o foo.s
	g++ -g -c foo.cpp -o foo.o
	readelf --debug-dump foo.o > readelf.log
	objdump -s --section=.debug_info foo.o > objdump.log

clean:
	rm -rf *.log *.s *.o
