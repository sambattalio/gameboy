// main.c

#include <stdio.h>
#include "proc.h"
#include "cart.h"
#include "video.h"

int main(int argc, char **argv) {

    Proc* processor = proc_create();

    Cart* cartridge = cart_create("../roms/Dr. Mario (World).gb");

    cart_load(cartridge, processor);

    Screen* screen = screen_create();

    pthread_t thread_id = dispatch_thread(screen, processor);

    while (1) {
        proc_read_word(processor);
    }

    // TODO signal the video thread to end here 
    
    // Wait for the video thread to end 
    pthread_join(thread_id, NULL);

    return 0; 
}