clean:
	@rm -rf *.so *.o *.a main

inner:
	gcc -shared -fPIC lib_inner.c -o libinner.so

outer_a:
	gcc -c lib_outer.c
	ar rcs libouter.a lib_outer.o

outer_alone_so:
	gcc -shared -fPIC lib_outer.c -o libouter.so

outer_inner_so:
	gcc -shared -fPIC lib_outer.c -L. -linner -o libouter.so

main_a: clean inner outer_a
	gcc -o main main.c libouter.a -L. -linner
	env LD_LIBRARY_PATH=. ./main

# same as main_a2
main_a2: clean inner outer_a
	gcc -o main main.c -L. -Wl,-static -louter -Wl,-Bdynamic -linner
	env LD_LIBRARY_PATH=. ./main

# libouter.so does not know libinner.so
main_so: clean inner outer_alone_so
	gcc -o main main.c -L. -louter -linner
	env LD_LIBRARY_PATH=. ./main

# executable without libinner.so, use LD_PRELOAD to run
main_so2: clean inner outer_alone_so
	gcc -o main main.c -L. -louter -Wl,--allow-shlib-undefined
	env LD_PRELOAD=./libinner.so LD_LIBRARY_PATH=. ./main

# libouter.so knows libinner.so
main_so3: clean inner outer_inner_so
	gcc -o main main.c -L. -louter -Wl,-rpath-link=.
	env LD_LIBRARY_PATH=. ./main

# same as main_so3
main_so4: clean inner outer_inner_so
	gcc -o main main.c -L. -louter -Wl,-rpath=.
	env LD_LIBRARY_PATH=. ./main

# same as main_so3
main_so5: clean inner outer_inner_so
	gcc -o main main.c -L. -louter -Xlinker -rpath .
	env LD_LIBRARY_PATH=. ./main

# same as main_so3
main_so6: clean inner outer_inner_so
	export LD_LIBRARY_PATH=. && gcc -o main main.c -L. -louter
	env LD_LIBRARY_PATH=. ./main
