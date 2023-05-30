# Refer to: http://www.kaizou.org/2015/01/linux-libraries.html
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

# static link libouter.a and dynamic link libinner.so
# it needs LD_LIBRARY_PATH for libinner.so search path
# ldd main
#   linux-vdso.so.1 (0x00007ffff2dc8000)
#   libinner.so => not found
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f02a7970000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007f02a7bbb000)
main_a: clean inner outer_a
	gcc -o main main.c libouter.a -L. -linner
	env LD_LIBRARY_PATH=. ./main

# same as main_a
# ldd main
#   linux-vdso.so.1 (0x00007fffed222000)
#   libinner.so => not found
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f2b50a10000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007f2b50c5c000)
main_a2: clean inner outer_a
	gcc -o main main.c -L. -Wl,-static -louter -Wl,-Bdynamic -linner
	env LD_LIBRARY_PATH=. ./main

# no link option during link, so it needs LD_LIBRARY_PATH
# ldd main
#   linux-vdso.so.1 (0x00007fffd73c6000)
#   libouter.so => not found
#   libinner.so => not found
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f74af570000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007f74af7b3000)
main: clean inner outer_alone_so
	gcc -o main main.c -L. -louter -linner
	env LD_LIBRARY_PATH=. ./main

# "-rpath-link" only affects compile process, not runtime,
# so it needs LD_LIBRARY_PATH
# ldd main
#   linux-vdso.so.1 (0x00007fffe0774000)
#   libouter.so => not found
#   libinner.so => not found
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f4637460000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007f46376a2000)
main_so: clean inner outer_alone_so
	gcc -o main main.c -L. -louter -linner -Wl,-rpath-link=.
	env LD_LIBRARY_PATH=. ./main

# "-rpath" specifies the library path, no need LD_LIBRARY_PATH
# ldd main
#   linux-vdso.so.1 (0x00007fffef8c8000)
#   libouter.so => ./libouter.so (0x00007f3252b10000)
#   libinner.so => ./libinner.so (0x00007f3252b00000)
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f32528d0000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007f3252b36000)
main_so1: clean inner outer_alone_so
	gcc -o main main.c -L. -louter -linner -Wl,-rpath=.
	./main

# "--allow-shlib-undefined" suppress the link error for libinner.so symbols,
# so, it needs LD_PRELOAD to load libinner.so in advance
# ldd main
#   linux-vdso.so.1 (0x00007fffc3dda000)
#   libouter.so => not found
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fcccec50000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007fcccee96000)
main_so2: clean inner outer_alone_so
	gcc -o main main.c -L. -louter -Wl,--allow-shlib-undefined
	env LD_PRELOAD=./libinner.so LD_LIBRARY_PATH=. ./main

# Different from "main_so", libouter.so knows libinner.so, so it doesn't
# need specify libinner.so during compile
# ldd main
#   linux-vdso.so.1 (0x00007fffe35b8000)
#   libouter.so => not found
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f1249120000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007f1249365000)
main_so3: clean inner outer_inner_so
	gcc -o main main.c -L. -louter -Wl,-rpath-link=.
	env LD_LIBRARY_PATH=. ./main

# Different from "main_so1", libinner.so cannot be found by main because libouter.so
# hides it
# it needs LD_LIBRARY_PATH
# ldd main
#   linux-vdso.so.1 (0x00007fffd6902000)
#   libouter.so => ./libouter.so (0x00007f0e15200000)
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f0e14fd0000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007f0e15224000)
#   libinner.so => not found
main_so4: clean inner outer_inner_so
	gcc -o main main.c -L. -linner -louter -Wl,-rpath=.
	env LD_LIBRARY_PATH=. ./main

# same as main_so4
# ldd main
#   linux-vdso.so.1 (0x00007ffff82f6000)
#   libouter.so => ./libouter.so (0x00007f9479930000)
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f9479700000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007f9479955000)
#   libinner.so => not found
main_so5: clean inner outer_inner_so
	gcc -o main main.c -L. -louter -Xlinker -rpath .
	env LD_LIBRARY_PATH=. ./main

# same as "-rpath-link" option
# ldd main
#   linux-vdso.so.1 (0x00007fffcda2e000)
#   libouter.so => not found
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fb0322e0000)
#   /lib64/ld-linux-x86-64.so.2 (0x00007fb03252a000)
main_so6: clean inner outer_inner_so
	export LD_LIBRARY_PATH=. && gcc -o main main.c -L. -louter
	env LD_LIBRARY_PATH=. ./main

debug:
	env LD_DEBUG=libs LD_LIBRARY_PATH=. ./main
