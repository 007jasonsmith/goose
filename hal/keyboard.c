#include "hal/keyboard.h"

#include "klib/debug.h"
#include "klib/types.h"

// Load the keymap file.
#define NOP '\0'
KeyboardKey keyboard_keymap[] = {
  { 0x00, "ERROR", NOP, NOP },
  #include "hal/en-us-keyboard.map"
};
const uint32 keyboard_keymap_size = sizeof(keyboard_keymap) / sizeof(KeyboardKey);
#undef NOP

// http://wiki.osdev.org/PS/2_Keyboard

volatile uint32 key_generation = 0;
volatile KeyPress last_keypress;

void keyboard_process(uint32 scancode) {
  // The most signifgant bit of a scancode is whether or not the key was released.
  bool key_released = (scancode & 0x80);
  scancode &= ~0x80;

  // EXPERIMENTAL - Keymap not finished yet.
  if (scancode > keyboard_keymap_size) {
    debug_log("Got unknown keyboard scancode[%d]", scancode);
    return;
  }

  // TODO(chrsmith): Implement atomic reads/writes for crying out loud!
  // TODO(chrsmith): Store in a lock-free ring buffer?
  key_generation++;
  last_keypress.key = keyboard_keymap[scancode];
  last_keypress.was_released = key_released;
}

void keyboard_getchar(char* c) {
  uint32 starting_generation = key_generation;
  debug_log("keyboard_getchar for generation %d", starting_generation);
  while (last_keypress.was_released ||
         last_keypress.key.c == '\0' ||
         key_generation == starting_generation) {
    // Wait for the right condition...
  }
  *c = last_keypress.key.c;
}
