CXX=gcc
CFLAGS=-g -Wall

.PHONY: all countdown clean

all: countdown

countdown: countdown.c
	$(CXX) $(CFLAGS) -o countdown countdown.c

clean:
	rm -f countdown.o countdown
