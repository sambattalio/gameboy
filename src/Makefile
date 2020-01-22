CC = gcc

CFLAGS = -std=c99 -Wall

all: main

main: main.o proc.o cart.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c helpers.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm main
	rm *.o

.PHONY: clean
