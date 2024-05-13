CC = gcc
CFLAGS = -Wall

all: user printer

user: user.c
	$(CC) $(CFLAGS) user.c -o user -lrt -lpthread

printer: printer.c
	$(CC) $(CFLAGS) printer.c -o printer -lrt -lpthread

clean:
	rm -f user printer