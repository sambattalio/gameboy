#ifndef VIDEO_H
#define VIDEO_H

#include "SDL2/SDL.h"
#include "pthread.h"

#include "proc.h"

#define SCREEN_H 160
#define SCREEN_W 144
#define MAX_SPRITES 40

// The gameboy screen buffer is larger than the visible screen
#define BACKGROUND_H 256
#define BACKGROUND_W 256

typedef struct {
    SDL_Window * screen;
    SDL_Renderer * renderer;
    SDL_Texture * texture;
    SDL_Event event;
} Screen;

typedef struct {
    Screen * s;
    Proc * p;
} VideoThreadArg;

Screen * screen_create();

pthread_t dispatch_thread(Screen * s, Proc * p);

void * video_thread(void *);

void render(Screen * s, Proc * p);

#endif
