#include <kernel.h> 
#include <stdlib.h>
#include <time.h> 
#include <malloc.h> 
#include <tamtypes.h> 
#include <math3d.h> 

#include <gsKit.h>
#include <gsInline.h>
#include <dmaKit.h>

#include <draw.h>
#include <draw3d.h>
#include <gsToolkit.h>
#include "graphics.h" 

int main(int argc, char *argv[]) {
    GSGLOBAL *gs = GUI_init(); 
    if (gs == NULL) {
        printf("Failed to initialize graphics or GUI.\n");
        return -1;
    }

    while(1) {
        GUI_next_frame(gs);
        flipScreen(gs); // Updates framebuffer and flips the screen. 
    }
}