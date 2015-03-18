// Interface for VGA mode 3, an 80x25 text mode.

#ifndef HAL_TEXT_UI_H_
#define HAL_TEXT_UI_H_

#include "klib/types.h"

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

 private:
  static bool initialized_;
};

}  // namespace hal

#endif  // HAL_TEXT_UI_H_
