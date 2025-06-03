/*

* input.h -- Endpoint for input handling in the PS2 Flight Computer.
* Author: Nick Janne 
* Date: 06-01-2025
* 
*/
#ifndef INPUT_H
#define INPUT_H
#include <tamtypes.h> 

//// A 32-bit bitmask for each button a DualShock/digital pad: 
typedef enum {
    INPUT_BTN_LEFT      = (1 << 0),
    INPUT_BTN_DOWN      = (1 << 1),
    INPUT_BTN_RIGHT     = (1 << 2),
    INPUT_BTN_UP        = (1 << 3),
    INPUT_BTN_START     = (1 << 4),
    INPUT_BTN_R3        = (1 << 5),
    INPUT_BTN_L3        = (1 << 6),
    INPUT_BTN_SELECT    = (1 << 7),
    INPUT_BTN_SQUARE    = (1 << 8),
    INPUT_BTN_CROSS     = (1 << 9),
    INPUT_BTN_CIRCLE    = (1 << 10),
    INPUT_BTN_TRIANGLE  = (1 << 11),
    INPUT_BTN_R1        = (1 << 12),
    INPUT_BTN_L1        = (1 << 13),
    INPUT_BTN_R2        = (1 << 14),
    INPUT_BTN_L2        = (1 << 15),
} InputButtonMask;

// Holds the current digital + analog state of the controller. 
typedef struct {
    u16               btns;       ///< 16‐bit: each bit = “0 if pressed, 1 if released”
    u8                ljoy_h;     ///< left stick X (0..255)
    u8                ljoy_v;     ///< left stick Y (0..255)
    u8                rjoy_h;     ///< right stick X (0..255)
    u8                rjoy_v;     ///< right stick Y (0..255)
    u8                left_p;     ///< pressure value (0..255) for L1, L2, etc. (if in press mode)
    u8                right_p;    ///< pressure for R1, R2
    u8                up_p;       ///< pressure for D-pad Up
    u8                down_p;     ///< pressure for D-pad Down
    u8                square_p;   ///< pressure for Square
    u8                cross_p;    ///< pressure for Cross (X)
    u8                circle_p;   ///< pressure for Circle
    u8                triangle_p; ///< pressure for Triangle
    // (Some SDK versions add extra padding bytes here; double‐check your libpad.h.)
} RawPadState;

typedef enum {
    INPUT_BTN_LEFT      = (1u << 0),   ///< D-pad Left
    INPUT_BTN_DOWN      = (1u << 1),   ///< D-pad Down
    INPUT_BTN_RIGHT     = (1u << 2),   ///< D-pad Right
    INPUT_BTN_UP        = (1u << 3),   ///< D-pad Up
    INPUT_BTN_START     = (1u << 4),   ///< Start
    INPUT_BTN_R3        = (1u << 5),   ///< R3 (stick click)
    INPUT_BTN_L3        = (1u << 6),   ///< L3 (stick click)
    INPUT_BTN_SELECT    = (1u << 7),   ///< Select
    INPUT_BTN_SQUARE    = (1u << 8),   ///< Square
    INPUT_BTN_CROSS     = (1u << 9),   ///< Cross (X)
    INPUT_BTN_CIRCLE    = (1u << 10),  ///< Circle
    INPUT_BTN_TRIANGLE  = (1u << 11),  ///< Triangle
    INPUT_BTN_R1        = (1u << 12),  ///< R1 shoulder button
    INPUT_BTN_L1        = (1u << 13),  ///< L1 shoulder button
    INPUT_BTN_R2        = (1u << 14),  ///< R2 shoulder (analog) button
    INPUT_BTN_L2        = (1u << 15),  ///< L2 shoulder (analog) button
    // (Additional bits beyond bit 15 are unused unless you add more features.)
} InputButtonMask;


#endif // INPUT_H