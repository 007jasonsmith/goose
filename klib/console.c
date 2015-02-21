#include "klib/console.h"

#include "klib/base.h"
#include "klib/types.h"
#include "sys/io.h"

// TODO(chris): Learn more about this.
// http://wiki.osdev.org/Text_UI

Window con_windows[NUM_WINDOWS];
uint8* const screen_buffer = (uint8*) 0x000B8000;

// Private methods (not found in header).
Window* con_win_get_active();
void con_win_print_char_active(char c);
void con_set_cursor(uint8 x, uint8 y);
void con_set_char(uint8 x, uint8 y, char c);
void con_set_color(uint8 x, uint8 y, Color foreground, Color background);
void con_win_set_focus(Window* win);
void con_win_scroll(Window* win);
void con_init_window(Window* win, const char* title,
                     uint8 x, uint8 y, uint8 width, uint8 height,
                     Color foreground, Color background);
size con_pos_to_idx(uint8 x, uint8 y);

// Returns the raw index into the screen_buffer. So screen_buffer[index] is the
// character, and screen_buffer[index + 1] is the formatting.
size con_pos_to_idx(uint8 x, uint8 y) {
  return (y * 80 + x) * 2;
}

Window* con_win_get_active() {
  for (int i = 0; i < NUM_WINDOWS; i++) {
    if (con_windows[i].has_focus) {
      return &con_windows[i];
    }
  }
  return null;
}

// Prints a character to the currently active window.
void con_win_print_char_active(char c) {
  Window* win = con_win_get_active();
  // Wrap the cursor as necessary.
  if (c == '\n' || win->cursor_col >= 80) {
    win->cursor_line++;
    win->cursor_col = 0;
  }
  // Scroll as necessary.
  if (win->cursor_line >= win->height - win->offset_y) {
    win->cursor_line = win->height - win->offset_y;
    con_win_scroll(win);
  }
  // Print.
  if (c != '\n') {
    con_set_char(win->cursor_col, win->cursor_line, c);
    win->cursor_col++;
    con_set_cursor(win->cursor_col, win->cursor_line);
  }
}

// Initialize the console. Clears the screen, initializes data structures, etc.
void con_initialize() {
  // Clear the string, defaulting to gray on black.
  const uint16 cell = (GRAY >> 8) | BLACK;
  for (int i = 0; i < 80 * 25; i++) {
    screen_buffer[i * 2] = cell;
  }

  // Initialize the kernel windows.
  con_init_window(&con_windows[WIN_OUTPUT], "Output",
                  0, 1, 40, 24, WHITE, BLUE);
  con_init_window(&con_windows[WIN_DEBUG], "Debug",
                  40, 1, 40, 24, BLACK, RED);

  // Disable the console blink. This also means we can use up to 16 colors.
  inb(0x3DA);
  outb(0x3C0, 0x30);
  inb(0x3C1);
  outb(0x3C0, 0x30);
}

// Writes the text to the window. Scrolling text as necessary.
void con_writeline(Window* win, const char* fmt, ...) {
  con_win_set_focus(win);

  base_printf(fmt, &con_win_print_char_active);

  // Handle an implicit '\n'.
  Window* active_win = con_win_get_active();
  active_win->cursor_line++;
  active_win->cursor_col = 0;
}

// Read text into the buffer, blocking until the return key is pressed.
// Output is echoed to the console.
void con_win_readline(Window* win, char* buffer, size buffer_size) {
  // TODO(chris): Implement me. This will be hard.
  win++;
  buffer++;
  buffer_size++;
}

void con_init_window(Window* win, const char* title,
                     uint8 x, uint8 y, uint8 width, uint8 height,
                     Color foreground, Color background) {
  win->title = title;

  win->offset_x = x;
  win->offset_y = y;
  win->width = width;
  win->height = height;

  win->foreground = foreground;
  win->background = background;

  // TODO(chris): Print the window title.

  // Clear the window region.
  for (; y < height; y++) {
    for (; x < width; x++) {
      con_set_color(x, y, foreground, background);
    }
  }

  win->has_focus = false;
  win->cursor_col = 0;
  win->cursor_line = 0;
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
  screen_buffer[index] = ((background & 0x0F) << 4) | (foreground & 0x0F);
}

void con_win_set_focus(Window* win) {
  Window* window_with_focus = con_win_get_active();
  window_with_focus->has_focus = false;
  win->has_focus = true;
}

void con_win_scroll(Window* win) {
  for (int y = win->height - win->offset_y; y <= win->offset_y - 1; y--) {
    for (int x = 0; x < win->width - win->offset_x; x++) {
        uint16 from = con_pos_to_idx(x, y);
        uint16 to = con_pos_to_idx(x, y - 1);
        screen_buffer[to] = screen_buffer[from];
    }
  }
}
