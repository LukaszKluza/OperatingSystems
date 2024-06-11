CC = gcc
CFLAGS = -Wall

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server -lpthread


client: client.c
	$(CC) $(CFLAGS) client.c -o client -lpthread

clear:
	rm -f server client



