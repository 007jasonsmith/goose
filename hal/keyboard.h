// Jankey device-driver-like-thing for the keyboard.

#ifndef HAL_KEYBOARD_H_
#define HAL_KEYBOARD_H_

#include "klib/types.h"

// TODO(chris): Support capslock.
// TODO(chris): Support shift+x for capitals, etc.
typedef struct {
  bool is_printable;
  char c;  // Undefined if the KeyPress is not printable.
  bool was_released;
} KeyPress;

// Handle the keyboard interrupt taking the scancode and moving into
// internal buffers.
void keyboard_process(uint32 scancode);

// Wait until a printable character is pressed, at which time key
// will be populated.
void keyboard_getchar(KeyPress* key);

#endif  // HAL_KEYBOARD_H_
