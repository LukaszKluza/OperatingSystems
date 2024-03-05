all:
	gcc -c collatz.c
	ar rcs libcollatz.a collatz.o

	gcc -fPIC -c collatz.c
	gcc -shared collatz.o -o libcollatz.so

	gcc main.c libcollatz.a -o main_start
	gcc main.c -L. -lcollatz -Wl,-rpath=. -o main_wspoldz
	gcc main_2.c -o main_dyn -ldl

clean:
	rm -f *.o
	rm -f *.so*
	rm -f *.a