/*

* input_manager.h -- Input manager for the PS2 flight computer
*                       Can handle DualShocks and Flight Avionics :)
* Author: Nick Janne 
* Date: 06-05-2025
* 
*/
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
#define DUALSHOCK_CONTROLLER 1
#define AVIONICS_INPUT 2

typedef struct {
    int ports_active[2];
    int ports_mode[2];  
    int slot; 
} InputManagerConfig;

static InputManagerConfig active_config = { // applied with default options. 
    .ports_active = {1, 0}, // Port 0 active, Port 1 inactive
    .ports_mode = {DUALSHOCK_CONTROLLER, 0}, // Port 0 uses DualShock, Port 1 unused
    .slot = 0 // Default slot
};

#ifdef DUALSHOCK_CONTROLLER 
static int waitPadReady(int port, int slot); // Function that blocks until pad is stable. 
static int initializePad(int port, int slot); 

#endif

static void loadIRXModules(void); 
int initInputManager(InputManagerConfig *config); // Initializes the input manager with the given configuration.
void printInputIfAvailable(void); // Prints the current input state if available. 

#endif // INPUT_MANAGER_H