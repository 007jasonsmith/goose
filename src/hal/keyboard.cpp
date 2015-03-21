#include "hal/keyboard.h"

#include "klib/debug.h"
#include "klib/types.h"

using hal::Keyboard::KeyboardKey;
using hal::Keyboard::KeyPress;

namespace {

// http://wiki.osdev.org/PS/2_Keyboard

// Load the keymap file.
#define NOP '\0'
const KeyboardKey keyboard_keymap[] = {
  { 0x00, "ERROR", NOP, NOP },
  #include "hal/en-us-keyboard.map"
};
const uint32 keyboard_keymap_size = sizeof(keyboard_keymap) / sizeof(KeyboardKey);
#undef NOP

// TODO(chris): Make volatile
uint32 key_generation = 0;
KeyPress last_keypress;

// Wait for any key to be pressed.
void WaitForKeypress();

void WaitForKeypress() {
  uint32 starting_generation = key_generation;
  while (!last_keypress.was_pressed ||
	 key_generation == starting_generation) {
    // Wait until SendScancode is called.
  }
}

}  // anonymous namespace

namespace hal {

namespace Keyboard {

void SendScancode(uint32 scancode) {
  // The most signifgant bit of a scancode is whether or not the key was released.
  bool key_pressed = !(scancode & 0x80);
  scancode &= ~0x80;

  if (scancode >= keyboard_keymap_size) {
    if (key_pressed) {
      klib::Debug::Log("Unknown key scancode[%d]", scancode);
    }
    return;
  }

  // TODO(chrsmith): Implement atomic reads/writes for crying out loud!
  // TODO(chrsmith): Store in a lock-free ring buffer?
  if (key_pressed) {
    key_generation++;
  }
  last_keypress.key = keyboard_keymap[scancode];
  last_keypress.was_pressed = key_pressed;
}

void GetCharacterKeypress(char* c) {
  do {
    WaitForKeypress();
  } while (last_keypress.key.c == '\0');
  *c = last_keypress.key.c;
}

KeyPress GetKeypress() {
  WaitForKeypress();
  return last_keypress;
}

KeyboardKey& KeyboardKey::operator=(const KeyboardKey& other) {
  this->scancode = other.scancode;
  this->name = other.name;
  this->c = other.c;
  this->shifted_c = other.shifted_c;
  return *this;
}

KeyPress::KeyPress() {}

KeyPress::KeyPress(const KeyPress& other) {
  this->key = other.key;
  this->was_pressed = other.was_pressed;
}


}  // namespace Keyboard

}  // namespace hal
