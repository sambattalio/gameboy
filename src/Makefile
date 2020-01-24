CC = gcc

CFLAGS = -std=c99 -Wall -lpthread -D_THREAD_SAFE -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2
DEBUG_FLAGS = -DDEBUG

CORE_FILES = main.c proc.c cart.c helpers.c memory.c video.c

CORE_OBJECTS = $(patsubst %, %, $(CORE_FILES:.c=.o))
DEBUG_OBJECTS = $(patsubst %, debug_%, $(CORE_FILES:.c=.o))

all: main

main: $(CORE_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

test: test.o helpers.o
	$(CC) $(CFLAGS) $^ -o $@
	./test
	rm test

%.o: %.c helpers.h
	$(CC) $(CFLAGS) -c $< -o $@

debug: $(DEBUG_OBJECTS)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@
	@mv debug main

debug_%.o: %.c helpers.h
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

clean:
	rm -f main
	rm -f *.o

.PHONY: clean
