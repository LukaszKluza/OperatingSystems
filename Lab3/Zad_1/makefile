CC = gcc
CFLAGS = -Wall

all: reverse main

.PHONY: all clean

reverse:
	$(CC) $(CFLAGS) -c reverse.c
	ar rcs libreverse.a reverse.o

main: main.c libreverse.a
	$(CC) $(CFLAGS) main.c libreverse.a -o main_fin

clean:
	rm -f *.o *.so* *.a main_fin
