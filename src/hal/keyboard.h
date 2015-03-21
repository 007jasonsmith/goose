// Jankey device-driver-like-thing for the keyboard.

#ifndef HAL_KEYBOARD_H_
#define HAL_KEYBOARD_H_

#include "klib/types.h"

// TODO(chris): Move to a namespace. Refactor.
// TODO(chris): Support capslock.
// TODO(chris): Support shift+x for capitals, etc.

namespace hal {

namespace Keyboard {

struct KeyboardKey {
  uint32 scancode;
  const char* name;
  char c;
  char shifted_c;

  KeyboardKey& operator=(const KeyboardKey&);
};

struct KeyPress {
  KeyPress();
  KeyPress(const KeyPress&);

  KeyboardKey key;
  bool was_pressed;
};

// Handle the keyboard interrupt taking the scancode and moving into
// internal buffers.
void SendScancode(uint32 scancode);

// Wait until a printable character is pressed.
void GetCharacterKeypress(char* c);

// Wait until the next key is pressed.
KeyPress GetKeypress();

}  // namespace Keyboard

}  // namespace hal

#endif  // HAL_KEYBOARD_H_
