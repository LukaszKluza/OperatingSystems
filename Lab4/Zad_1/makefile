CC = gcc
CFLAGS = -Wall

all: child main

child: child.c
	$(CC) $(CFLAGS) child.c -o child

main: main.c
	$(CC) $(CFLAGS) main.c -o main_fin

clean:
	rm -f child main_fin