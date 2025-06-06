/*

* input_manager.c -- Input manager for the PS2 flight computer
*                       Can handle DualShocks and Flight Avionics :)
* Author: Nick Janne 
* Date: 06-05-2025
* 
*/

#include <tamtypes.h> 
#include <kernel.h> 
#include <sifrpc.h> 
#include <loadfile.h> 
#include <stdio.h> 
#include "input_manager.h" 
#include "libpad.h" 
#include "irx_embed_symbols.h"
#define DUALSHOCK_CONTROLLER 1


#ifdef DUALSHOCK_CONTROLLER 
static char padBuf[256] __attribute__((aligned(64))); // Buffer for pad data
static char actAlign[6]; // Buffer for actuator alignment
static int actuators; // Number of actuators detected

static int waitPadReady(int port, int slot)
{
    int state;
    int lastState;
    char stateString[16];

    state = padGetState(port, slot);
    lastState = -1;
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
            printf("Please wait, pad(%d,%d) is in state %s\n",
                       port, slot, stateString);
        }
        lastState = state;
        state=padGetState(port, slot);
    }
    // Were the pad ever 'out of sync'?
    if (lastState != -1) {
        printf("Pad OK!\n");
    }
    return 0;
} // Function that blocks until pad is stable. 


static int
initializePad(int port, int slot)
{

    int ret;
    int modes;
    int i;

    waitPadReady(port, slot);

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
    printf("The device has %d modes\n", modes);

    if (modes > 0) {
        printf("( ");
        for (i = 0; i < modes; i++) {
            printf("%d ", padInfoMode(port, slot, PAD_MODETABLE, i));
        }
        printf(")");
    }

    printf("It is currently using mode %d\n",
               padInfoMode(port, slot, PAD_MODECURID, 0));

    // If modes == 0, this is not a Dual shock controller
    // (it has no actuator engines)
    if (modes == 0) {
        printf("This is a digital controller?\n");
        return 1;
    }

    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);
    if (i >= modes) {
        printf("This is no Dual Shock controller\n");
        return 1;
    }

    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
        printf("This is no Dual Shock controller??\n");
        return 1;
    }

    printf("Enabling dual shock functions\n");

    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
    printf("infoPressMode: %d\n", padInfoPressMode(port, slot));

    waitPadReady(port, slot);
    printf("enterPressMode: %d\n", padEnterPressMode(port, slot));

    waitPadReady(port, slot);
    actuators = padInfoAct(port, slot, -1, 0);
    printf("# of actuators: %d\n",actuators);

    if (actuators != 0) {
        actAlign[0] = 0;   // Enable small engine
        actAlign[1] = 1;   // Enable big engine
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady(port, slot);
        printf("padSetActAlign: %d\n",
                   padSetActAlign(port, slot, actAlign));
    }
    else {
        printf("Did not find any actuators.\n");
    }

    waitPadReady(port, slot);

    return 1;
}

#endif

int initInputManager(InputManagerConfig *config) {
    int ret; 

    // Initialize the SIF RPC system
    SifInitRpc(0);
    
    // Load necessary IRX modules
    loadIRXModules();

    // Initialize the pad subsystem
    padInit(0); 

    printf("Initializing Input Manager...\n");
    printf("PortMax: %d\n", padGetPortMax());
    printf("SlotMax: %d\n", padGetSlotMax(0));

    // Check if the configuration exists. If it does, replace active_config with it. 
    if (config != NULL) {
        active_config = *config;
    } else {
        printf("No configuration provided, using default.\n");
    }
    
    for (int i = 0; i < 2; i++) {
        if (active_config.ports_active[i]) {
            // Open the pad port for each active port
            if (active_config.ports_mode[i] == DUALSHOCK_CONTROLLER) {
                printf("Opening DualShock controller on port %d, slot %d...\n", i, active_config.slot);
                if ((ret = padPortOpen(i, active_config.slot, padBuf)) == 0) {
                    printf("padOpenPort failed for port %d: %d\n", i, ret);
                    return -1;
                } else {
                    printf("Pad port %d opened successfully.\n", i);
                    if (!initializePad(i, active_config.slot)) {
                        printf("Failed to initialize pad on port %d, slot %d.\n", i, active_config.slot);
                        return -1;
                    } else {
                        printf("Pad on port %d initialized successfully.\n", i);
                    }
                }
            } else {
                printf("Avionics Input not yet supported on port %d.\n", i);
            }
        }
    }

    // Print out the maximum number of ports and slots available
    printf("PortMax: %d\n", padGetPortMax());
    printf("SlotMax: %d\n", padGetSlotMax(0));
    return 0; 
}

void printInputIfAvailable() {
    static u32 old_pad_1 = 0; 
    static u32 old_pad_2 = 0;
    struct padButtonStatus buttons;
    u32 paddata;
    u32 new_pad; 
    u32 old_pad; 

    for (int i = 0; i < 2; i++) {
        old_pad = (i == 0) ? old_pad_1 : old_pad_2; // Use different old_pad for each port
        if (active_config.ports_active[i]) {
            int port = i;
            int slot = active_config.slot;
            int state = padGetState(port, slot);
            if (state == PAD_STATE_STABLE || state == PAD_STATE_FINDCTP1) {
                if (padRead(port, slot, &buttons) != 0) {
                    paddata = 0xffff ^ buttons.btns;
                    new_pad = paddata & ~old_pad;
                    old_pad = paddata;

                    // Print the button states
                    if (new_pad & PAD_LEFT) printf("Port %d, Slot %d: LEFT\n", port, slot);
                    if (new_pad & PAD_DOWN) printf("Port %d, Slot %d: DOWN\n", port, slot);
                    if (new_pad & PAD_RIGHT) printf("Port %d, Slot %d: RIGHT\n", port, slot);
                    if (new_pad & PAD_UP) printf("Port %d, Slot %d: UP\n", port, slot);
                    if (new_pad & PAD_START) printf("Port %d, Slot %d: START\n", port, slot);
                    if (new_pad & PAD_R3) printf("Port %d, Slot %d: R3\n", port, slot);
                    if (new_pad & PAD_L3) printf("Port %d, Slot %d: L3\n", port, slot);
                    if (new_pad & PAD_SELECT) printf("Port %d, Slot %d: SELECT\n", port, slot);
                    if (new_pad & PAD_SQUARE) printf("Port %d, Slot %d: SQUARE\n", port, slot);
                    if (new_pad & PAD_CROSS) printf("Port %d, Slot %d: CROSS\n", port, slot);
                    if (new_pad & PAD_CIRCLE) printf("Port %d, Slot %d: CIRCLE\n", port, slot);
                    if (new_pad & PAD_TRIANGLE) printf("Port %d, Slot %d: TRIANGLE\n", port, slot);
                    if (new_pad & PAD_R1) printf("Port %d, Slot %d: R1\n", port, slot);
                    if (new_pad & PAD_L1) printf("Port %d, Slot %d: L1\n", port, slot);
                    if (new_pad & PAD_R2) printf("Port %d, Slot %d: R2\n", port, slot);
                    if (new_pad & PAD_L2) printf("Port %d, Slot %d: L2\n", port, slot);

                } else {
                    printf("Failed to read pad data for port %d, slot %d.\n", port, slot);
                }
            } else {
                printf("Pad on port %d, slot %d is not stable.\n", port, slot);
            }
            // Update the old_pad for the next iteration
            if (i == 0) {
                old_pad_1 = old_pad; // Update for port 0
            } else {
                old_pad_2 = old_pad; // Update for port 1
            }
        }
    }
}

static void loadIRXModules() {
    int ret; 

    SifExecModuleBuffer(sio2man_irx, size_sio2man_irx, 0, NULL, &ret); 
    if (ret < 0) {
        printf("sifLoadModule sio failed: %d\n", ret);
        SleepThread();
    }

    #ifdef DUALSHOCK_CONTROLLER
    SifExecModuleBuffer(padman_irx, size_padman_irx, 0, NULL, &ret);
    if (ret < 0) {
        printf("sifLoadModule pad failed: %d\n", ret);
        SleepThread();
    }
    #endif
}
