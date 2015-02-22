#include "hal/keyboard.h"

#include "klib/types.h"

// Bake the keyboard map directly into the kernel.
#define ESCAPE "Escape"
#define KEY_1  "Key 1"
#define KEY_2  "Key 2"
#define KEY_3  "Key 3"
#define KEY_4  "Key 4"

typedef struct {
  uint32 scancode;
  const char* name;
  char c;
  char shifted_c;
} KeyMapChar;

#define NOP '\0'
KeyMapChar keyboard_keymap[] = {
  { 0x00, "ERROR", NOP, NOP },
  #include "hal/en-us-keyboard.map"
};

// http://wiki.osdev.org/PS/2_Keyboard

volatile KeyPress last_keypress;

void keyboard_process(uint32 scancode) {
  // The most signifgant bit of a scancode is whether or not the key was released.
  bool key_released = (scancode & 0x80);
  scancode &= ~0x80;

  // TODO(chrsmith): Implement atomic reads/writes for crying out loud!
  // lock-free ring buffer.
  // IDEA: Clang's list of node types, expression types.
  last_keypress.was_released = key_released;
  // EXPERIMENTAL.
  last_keypress.c = keyboard_keymap[scancode].c;
  last_keypress.is_printable = true;
}

void keyboard_getchar(KeyPress* key) {
  while (last_keypress.was_released) {
    // Wait until a key was _pressed_.
  }
  // TODO(chrsmith): memcpy?
  key->is_printable = last_keypress.is_printable;
  key->c = last_keypress.c;
  key->was_released = last_keypress.was_released;
}
