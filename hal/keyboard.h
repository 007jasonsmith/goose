// Jankey device-driver-like-thing for the keyboard.

#ifndef HAL_KEYBOARD_H_
#define HAL_KEYBOARD_H_

#include "klib/types.h"

// TODO(chris): Support capslock.
// TODO(chris): Support shift+x for capitals, etc.

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
void keyboard_process(uint32 scancode);

// Wait until a printable character is pressed.
void keyboard_get_char(char* c);

// Wait until the next key is pressed.
KeyPress keyboard_get_keypress();

#endif  // HAL_KEYBOARD_H_
