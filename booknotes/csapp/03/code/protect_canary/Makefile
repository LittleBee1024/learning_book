o:
	gcc -c canary.c -o canary.o
	objdump -d canary.o

clean:
	rm -rf *.o

o2:
	gcc -fno-stack-protector -c canary.c -o canary_no_stack_protector.o
	objdump -d canary_no_stack_protector.o

s:
	gcc -S canary.c
