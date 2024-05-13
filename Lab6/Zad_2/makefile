CC = gcc
CFLAGS = -Wall

all: calculate manage

calculate: calculate.c
	$(CC) $(CFLAGS) calculate.c -o calculate


manage: manage.c
	$(CC) $(CFLAGS) manage.c -o manage

clean:
	rm -f calculate manage in_pipe out_pipe