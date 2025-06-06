#include <kernel.h> 
#include <stdlib.h>
#include <time.h> 
#include <malloc.h> 
#include <tamtypes.h> 
#include <math3d.h> 

#include <gsKit.h>
#include <gsInline.h>
#include <dmaKit.h>
#include <sifrpc.h>

#include <draw.h>
#include <draw3d.h>
#include <gsToolkit.h>
#include "graphics.h" 

// This is a global flag that controls the main loop. 
// Marked voltaile so the compiler does not optimize it out.
// It can be modified from ps2client, which is how we can cause the ELF to exit and another to be loaded. 
volatile int keep_running = 1; 

int main(int argc, char *argv[]) {
    sceSifInitRpc(0); 
    GSGLOBAL *gs = GUI_init(); 
    if (gs == NULL) {
        printf("Failed to initialize graphics or GUI.\n");
        return -1;
    }
    printf("EE: Hello from the main function! Graphics initialized.\n");
    while(keep_running) {
        GUI_next_frame(gs);
        flipScreen(gs); // Updates framebuffer and flips the screen. 
    }

    printf("EE: Exiting main loop, returning to PS2Link...\n");

    return 0; 
}