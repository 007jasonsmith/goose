#include "klib/console.h"

#include "hal/keyboard.h"
#include "klib/strings.h"
#include "klib/types.h"
#include "sys/io.h"

// TODO(chris): Learn more about this.
// http://wiki.osdev.org/Text_UI

Window con_windows[NUM_WINDOWS];
uint8* const screen_buffer = (uint8*) 0x000B8000;
Window* active_window = &con_windows[0];

// Private methods (not found in header).
// TODO(chris): Global state is lame. Perhaps hide this by having
// con_win_print_char take a WindowId, and return a function pointer?
void con_set_cursor(uint8 x, uint8 y);
void con_set_char(uint8 x, uint8 y, char c);
void con_set_color(uint8 x, uint8 y, Color foreground, Color background);
size con_pos_to_idx(uint8 x, uint8 y);

// Returns the raw index into the screen_buffer. So screen_buffer[index] is the
// character, and screen_buffer[index + 1] is the formatting.
size con_pos_to_idx(uint8 x, uint8 y) {
  return (y * 80 + x) * 2;
}

void con_initialize() {
  // Disable the console blink. This also means we can use up to 16 colors.
  inb(0x3DA);
  outb(0x3C0, 0x30);
  inb(0x3C1);
  outb(0x3C0, 0x30);

  // Clear the string, defaulting to gray on black.
  const uint8 formatting = (BLACK << 4) | GRAY;
  for (int i = 0; i < 80 * 25; i++) {
    screen_buffer[i * 2] = '.';
    screen_buffer[i * 2 + 1] = formatting;
  }

  con_set_char(0, 0, 'G');
  con_set_char(1, 0, '2');

  con_set_cursor(79, 24);
}

void con_set_cursor(uint8 x, uint8 y) {
  uint16 pos = con_pos_to_idx(x, y);
  const uint16 kCommandPort = 0x3D4;
  const uint16 kDataPort = 0x3D5;
  const uint16 kHighByteCommand = 14;
  const uint16 kLowByteCommand = 15;
  outb(kCommandPort, kHighByteCommand);
  outb(kDataPort, ((pos >> 8) & 0x00FF));
  outb(kCommandPort, kLowByteCommand);
  outb(kDataPort,    pos & 0x00FF);
}

void con_set_char(uint8 x, uint8 y, char c) {
  uint16 index = con_pos_to_idx(x, y);
  screen_buffer[index] = c;
}

void con_set_color(uint8 x, uint8 y, Color foreground, Color background) {
  uint16 index = con_pos_to_idx(x, y);
  screen_buffer[index + 1] = ((background & 0x0F) << 4) | (foreground & 0x0F);
}
