CC = gcc
CFLAGS = -Wall -lc

all: main

.PHONY: all clean

main: main.c
	$(CC) $(CFLAGS) main.c -o main_fin

clean:
	rm -f *.o *.so* *.a main_fin

