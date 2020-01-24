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
    }
}

