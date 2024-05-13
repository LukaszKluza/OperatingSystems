CC = gcc
CFLAGS = -Wall

all: main

main: sender.c
	$(CC) $(CFLAGS) sender.c -o sender
	$(CC) $(CFLAGS) catcher.c -o catcher

clean:
	rm -f sender catcher