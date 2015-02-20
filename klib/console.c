#include "klib/console.h"

#include "klib/base.h"
#include "klib/string.h"
#include "klib/types.h"

// TODO(chris): Learn more about this.
// http://wiki.osdev.org/Text_UI

// Keep two pointers, as writing two bytes at a time may be more efficient.
uint8* const screen_buffer = (uint8*) 0x000B8000;
uint16* const screen_buffer16 = (uint16*) 0x000B8000;

// Initialize the console. Clears the screen, initializes data structures, etc.
void con_initialize() {
  // Gray on black.
  const uint16 cell = 8;
  for (int i = 0; i < 80 * 25; i++) {
    screen_buffer16[cell] = cell;
  }
}

// Writes the text to the window. Scrolling text as necessary.
void con_writeline(const Window& win, const char* fmt, ...) {
}

// Read text into the buffer, blocking until the return key is pressed.
// Output is echoed to the console.
void con_win_readline(const Window& win, char* buffer, size buffer_size) {
}

void con_init_window(Window* win, const char* title,
                     uint8 x, uint8 y, uint8 width, uint8 height,
                     Color foreground, Color background) {
}

void con_set_cursor(uint8 x, uint8 y) {
}

void con_set_char(uint8 x, uint8 y, char c) {
}

void con_set_color(uint8 x, uint8 y, Color foreground, Color background) {
}

void con_win_set_focus(Window* win) {
}

void con_win_scroll(Window* win) {
}
