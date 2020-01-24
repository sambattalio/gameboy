CC = gcc

CFLAGS = -std=c99 -Wall -lpthread
DEBUG_FLAGS = -std=c99 -Wall -DDEBUG -lpthread
SDL_FLAGS = -D_THREAD_SAFE -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2

all: main

main: main.o proc.o cart.o helpers.o memory.o video.o
	$(CC) $(CFLAGS) $(SDL_FLAGS) $^ -o $@

test: test.o helpers.o
	$(CC) $(CFLAGS) $^ -o $@
	./test
	rm test

%.o: %.c helpers.h
	$(CC) $(CFLAGS) -c $< -o $@

debug: main_debug.o proc_debug.o cart_debug.o helpers_debug.o memory_debug.o video_debug.o
	$(CC) $(CFLAGS) $^ -o $@
	@mv debug main

%_debug.o: %.c helpers.h
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

clean:
	rm main
	rm *.o

.PHONY: clean
