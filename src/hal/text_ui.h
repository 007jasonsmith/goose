// Interface for VGA mode 3, an 80x25 text mode.

#ifndef HAL_TEXT_UI_H_
#define HAL_TEXT_UI_H_

#include "klib/print.h"
#include "klib/types.h"
#include "klib/type_printer.h"

namespace hal {

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

  uint8 OffsetX() const;
  uint8 OffsetY() const;

 private:
  uint8 offset_x_;
  uint8 offset_y_;
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

  template<typename... Args>
  static void Print(const char* msg, uint8 x, uint8 y, Args... args) {
    TextUIOutputFn fn(x, y);
    klib::print(msg, &fn, args...);
  }

 private:
  static bool initialized_;
  static bool show_cursor_;
  static uint8 cursor_x_;
  static uint8 cursor_y_;
};

}  // namespace hal

#endif  // HAL_TEXT_UI_H_
