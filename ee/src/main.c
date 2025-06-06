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
#include "input_manager.h" 

// This is a global flag that controls the main loop. 
// Marked voltaile so the compiler does not optimize it out.
// It can be modified from ps2client, which is how we can cause the ELF to exit and another to be loaded. 
volatile int keep_running = 1; 


int main(int argc, char *argv[])
{
    /* --- subsystems ----------------------------------------------------- */
    if (initInputManager(NULL) != 0) {
        printf("Pad init failed\n");
        return -1;
    }
    GSGLOBAL *gs = GUI_init();
    if (!gs) {
        printf("Graphics init failed\n");
        return -1;
    }
    printf("EE: init ok — entering main loop\n");


    while (keep_running) {
        printInputIfAvailable();
        GUI_next_frame(gs);
        flipScreen(gs);
    }

    printf("EE: bye!\n");
    return 0;
}