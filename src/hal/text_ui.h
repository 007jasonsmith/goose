// Interface for VGA mode 3, an 80x25 text mode.

#ifndef HAL_TEXT_UI_H_
#define HAL_TEXT_UI_H_

#include "klib/print.h"
#include "klib/types.h"
#include "klib/type_printer.h"

namespace hal {

// A offset into the screen.
struct Offset {
  Offset();
  Offset(uint8 x, uint8 y);
  uint8 x;
  uint8 y;
};

// A region on the screen.
struct Region {
  Region(uint8 x, uint8 y, uint8 width, uint8 height);
  Offset offset;
  uint8 width;
  uint8 height;
};

enum class Color {
  Black   = 0,
  Blue    = 1,
  Green   = 2,
  Cyan    = 3,
  Red     = 4,
  Magenta = 5,
  Brown   = 6,
  Gray    = 7,

  // Colors only available if blinking mode disabled.
  LightGray    = 8,
  LightBlue    = 9,
  LightGreen   = 10,
  LightCyan    = 11,
  LightRed     = 12,
  LightMagenta = 13,
  LightBrown   = 14,
  White        = 15
};

class TextUIOutputFn : public klib::IOutputFn {
 public:
  TextUIOutputFn(uint8 x, uint8 y);

  virtual void Print(char c);

 private:
  Offset offset_;
};

class TextUI {
 private:
  // Do not call. Static utility class.
  TextUI();

 public:
  static bool IsInitialized();
  static void Initialize();

  static void SetCursor(uint8 x, uint8 y);
  static void SetChar(uint8 x, uint8 y, char c);
  static void SetColor(uint8 x, uint8 y, Color foreground, Color background);

  static void ShowCursor(bool show);

  static void Scroll(const Region& region);

  template<typename... Args>
  static void Print(const char* msg, uint8 x, uint8 y, Args... args) {
    TextUIOutputFn fn(x, y);
    klib::print(msg, &fn, args...);
  }

 private:
  static bool initialized_;
  static bool show_cursor_;
  static Offset cursor_;
};

}  // namespace hal

#endif  // HAL_TEXT_UI_H_
