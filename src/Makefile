CC = gcc

CFLAGS = -std=c99 -Wall
DEBUG_FLAGS = -std=c99 -Wall -DDEBUG

all: main

main: main.o proc.o cart.o helpers.o
	$(CC) $(CFLAGS) $^ -o $@

test: test.o helpers.o
	$(CC) $(CFLAGS) $^ -o $@
	./test
	rm test

%.o: %.c helpers.h
	$(CC) $(CFLAGS) -c $< -o $@

debug: main_debug.o proc_debug.o cart_debug.o helpers_debug.o
	$(CC) $(CFLAGS) $^ -o $@
	@mv debug main

%_debug.o: %.c helpers.h
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

clean:
	rm main
	rm *.o

.PHONY: clean
