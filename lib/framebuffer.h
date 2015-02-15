#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "lib/types.h"

typedef int8_t FbColor;

// There are eight colors, which can be optionally set to have a "bright bit".
// (Which covers values 8-15.) See:
// http://wiki.osdev.org/VGA_Hardware
// http://wiki.osdev.org/Printing_To_Screen
// http://wiki.osdev.org/Text_UI
#define FB_BLACK    0
#define FB_BLUE     1
#define FB_GREEN    2
#define FB_CYAN     3
#define FB_RED      4
#define FB_MAGENTA  5
#define FB_BROWN    6
#define FB_LGREY    7

#define FB_GRAY     8
#define FB_LBLUE    9
#define FB_LGREEN   10
#define FB_LCYAN    11
#define FB_LRED     12
#define FB_LMAGENTA 13
#define FB_LBROWN   14
#define FB_WHITE    15

/**
 * Clears the screen.
 */
void fb_clear(void);

/**
 * Moves the cursor of the framebuffer to the given position.
 *
 * @param pos The new position of the cursor.
 */
void fb_move_cursor(uint16_t pos);

/**
 * Disables VGA blinking, enabling "bright" colors for the background.
 */
void fb_disable_blink(void);

/**
 * Prints a string to the current window.
 */
void fb_println(const char* buf);

#endif  // FRAMEBUFFER_H_
