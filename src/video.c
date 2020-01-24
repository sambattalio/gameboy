#include "video.h"

Screen * screen_create() {
    Screen * s = calloc(1, sizeof(Screen));

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "error in starting sdl");
        exit(1);
    }
    
    SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &(s->screen), &(s->renderer));
    SDL_SetWindowTitle(s->screen, "ChadBoy");

    if (!s->screen) {
        fprintf(stderr, "sdl could not create screen");
        exit(1);
    }
    return s;
}

pthread_t dispatch_thread(Screen * s, Proc * p) {
    // Creates the thread to run the video process, pthread_join should be called in the caller to this
    pthread_t thread_id;
    VideoThreadArg * arg = calloc(1, sizeof(VideoThreadArg));
    arg->s = s;
    arg->p = p;

    pthread_create(&thread_id, NULL, video_thread, arg);
    return thread_id;
}

void * video_thread(void * varg) {
    /* Video thread that handles the while loop for the video displaying */
    VideoThreadArg * arg = (VideoThreadArg *) varg;

    while (1) {
        // TODO do the video updating in here
        // TODO make sure if we are accessing data in here from proc, it is going to not have 
        // any concurrency issues
        // NOTE - EVENT POLLING CANNOT HAPPEN IN A THREAD ON OSX
    }
}

void render(Screen * s, Proc * p) {
    uint32_t pixels[SCREEN_W * SCREEN_H] = {0};

    // TODO figure out how to decide what to render before updating screen 

    SDL_UpdateTexture(s->texture, NULL, pixels, SCREEN_W * sizeof(Uint32));
    SDL_RenderClear(s->renderer);
    SDL_RenderCopy(s->renderer, s->texture, NULL, NULL);
    SDL_RenderPresent(s->renderer);
}

uint32_t get_pixel_value(uint8_t value) {
    /* Switches the 2 bit color value from memory to a hex value for SDL to render */
    switch (value) {
        case 0: 
            return 0xFFFFFF;
        case 1:
            return 0xC0C0C0;
        case 2:
            return 0x606060;
        default:
            return 0;
    }
}
