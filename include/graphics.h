/*

* graphics.h -- Simplified graphics handling for PS2 Flight Computer 
* Author: Nick Janne 
* Date: 06-01-2025
* 
*/
#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <gsKit.h> 
#include <gsToolkit.h>
#include <gsInline.h>
#include <dmaKit.h>
#include <draw.h>
#include <draw3d.h>
#include <malloc.h>
#include "physics.h" 

// USEFUL COLOR MACROS 
#define Black GS_SETREG_RGBAQ(0x00, 0x00, 0x00, 0x80, 0x00)
#define White GS_SETREG_RGBAQ(0xFF, 0xFF, 0xFF, 0x00, 0x00)
#define Red GS_SETREG_RGBAQ(0xFF, 0x00, 0x00, 0x00, 0x00)
#define Green GS_SETREG_RGBAQ(0x00, 0xFF, 0x00, 0x00, 0x00)
#define Blue GS_SETREG_RGBAQ(0x00, 0x00, 0xFF, 0x00, 0x00)
#define BlueTrans GS_SETREG_RGBAQ(0x00, 0x00, 0xFF, 0x40, 0x00)
#define RedTrans GS_SETREG_RGBAQ(0xFF, 0x00, 0x00, 0x60, 0x00)
#define GreenTrans GS_SETREG_RGBAQ(0x00, 0xFF, 0x00, 0x50, 0x00)
#define WhiteTrans GS_SETREG_RGBAQ(0xFF, 0xFF, 0xFF, 0x50, 0x00)
#define GreenFont GS_SETREG_RGBAQ(0x80, 0xFF, 0x80, 0x80, 0x00)
#define RedFont GS_SETREG_RGBAQ(0xFF, 0x00, 0x00, 0x00, 0x00)
#define BlueFont GS_SETREG_RGBAQ(0x00, 0x00, 0xFF, 0x00, 0x00)
#define WhiteFont GS_SETREG_RGBAQ(0xFF, 0xFF, 0xFF, 0x00, 0x00)


/*
*
*                           External Functions
*
*/
// Flips the active framebuffer to show the rendered content. 
// To ensure this causes a re-render, you should have already 
// added something new to the inactive framebuffer's queue. 
void flipScreen(GSGLOBAL* gsGlobal); 

// Initializes the graphics subsystem for the PS2. Sets up the GS global 
// structure, configures DMA, and prepares the screen.
// @return Pointer to the initialized GSGLOBAL structure.
GSGLOBAL* init_graphics(); 

// Initializes the GUI subsystem.
// This function should be called once before the GUI is used. 
// This function calls init_graphics() to set up the graphics system before doing anything else. 
// Can be used instead of init_graphics() and returns the same GSGLOBAL pointer. 
GSGLOBAL* GUI_init();

// Draws the next frame for the GUI. Calls sub-functions that draw 
// individual elements of the GUI, and and updates relevant values 
// based on state changes of the vehicle. TODO: Add acceptance of state. 
void GUI_next_frame(GSGLOBAL* gs); 

/*
*
*                           Helper Functions
*
*/
// Draws the title and current time at the top of the screen inside GUI. 
static void GUI_draw_title_and_time(GSGLOBAL* gs);

// Draws the bottom boxes for the GUI. 
static void GUI_draw_bottom_boxes(GSGLOBAL *gs); 
static void compute_all_box_positions(GSGLOBAL *gs);

// Gets positions of the ith box in the GUI. 
static void get_box_position(int i, int *out_x1, int* out_y1, int *out_x2, int* out_y2);

// Draws an arrow inside the box at index i. 
// with orientation specified by yaw, pitch, and roll angles. 
static void GUI_draw_arrow(GSGLOBAL *gs, int i, float yawDeg, float pitchDeg, float rollDeg);

// Converts an array of "normalized" float vertices into fixed-point screen coordinates, 
// writing them to the output array. Returns 0 on success, -1 on failure. 
/// @param output    Pre-allocated array of `vertex_f_t` of length `count`.
/// @param gsGlobal  GSKit global (contains Width, Height, PSMZ).
/// @param count     Number of vertices in `vertices` and `output`.
/// @param vertices  Input array of normalized floats (object→clip after MVP).
static int gsKit_convert_xyz(vertex_f_t *output, GSGLOBAL* gsGlobal, int count, vertex_f_t *vertices);

#endif // GRAPHICS_H