// Goose will not support consoles as a first-class citizen. The console
// functionality isn't something that will get much investment, because Goose
// isn't intended to be installed on computers with keyboards; or monitors;
// or even be used by humans directly.
//
// However, as a kernel bootstrapping and debugging step, a console is necessary.
//
// The console is a text mode (80x25) console that will allow users to
// read/writetext. In addition it will support beautiful 16-color CGA graphs.
// (Think of Turbo C++.)
//
// The console will have a fixed, named set of "windows" that are defined in the
// kernel. For example "task output", "debug log", and "memory usage". These
// windows will each be a Window struct.
//
// For the initialize release of klib, Windows may not overlap. A future version
// may give Window objects their own buffers for text, and the ability to redraw
// themselves.
#ifndef SYS_CONSOLE_H_
#define SYS_CONSOLE_H_

#include "klib/base.h"
#include "klib/types.h"

typedef uint8 Color;
// There are eight colors, which can be optionally set to have a "bright bit".
// (Which covers values 8-15.) See:
// http://wiki.osdev.org/VGA_Hardware
// http://wiki.osdev.org/Printing_To_Screen
// http://wiki.osdev.org/Text_UI
#define BLACK    0
#define BLUE     1
#define GREEN    2
#define CYAN     3
#define RED      4
#define MAGENTA  5
#define BROWN    6
#define GRAY     7

#define LGRAY    8
#define LBLUE    9
#define LGREEN   10
#define LCYAN    11
#define LRED     12
#define LMAGENTA 13
#define LBROWN   14
#define WHITE    15

typedef struct {
  const char* title;

  // 0-indexed offset for the top-left of the Window.
  // Text will be one character smaller, to account
  // for the border.
  // Width and height are not 0-indexed, so be careful.
  uint8 offset_x;
  uint8 offset_y;
  uint8 width;
  uint8 height;

  // If set, the window will have a border of chrome that will
  // reduce the printable area and generally make implementation
  // more difficult.
  bool has_border;

  Color foreground;
  Color background;

  // Where the current cursor is. As text is written, it "moves"
  // to the bottom right. Once there, it will scroll the text
  // up.
  uint8 cursor_col;
  uint8 cursor_line;
} Window;

// Global windows
typedef int WindowId;
#define HEADER_WIN  (WindowId) 0
#define OUTPUT_WIN  (WindowId) 1
#define DEBUG_WIN   (WindowId) 2
#define NUM_WINDOWS 3

// Initialize the console. Clears the screen, initializes data structures, etc.
void con_initialize();

// Writes the text to the window. Scrolling text as necessary.
void con_writeline(WindowId win, const char* fmt, ...);
void con_writeline_va(WindowId win, const char* fmt, va_list args);

// Read text into the buffer, blocking until the return key is pressed.
// Output is echoed to the console.
void con_win_readline(WindowId win, char* buffer, size buffer_size);

#endif  // SYS_CONSOLE_H_
