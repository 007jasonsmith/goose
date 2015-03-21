#include "hal/text_ui.h"

#include "klib/types.h"
#include "sys/io.h"

// See: http://wiki.osdev.org/Text_UI

namespace {

uint8* const screen_buffer = (uint8*) 0x000B8000;

// Returns the raw index into the screen_buffer. So screen_buffer[index] is the
// character, and screen_buffer[index + 1] is the formatting.
size PosToIndex(uint8 x, uint8 y) {
  return (y * 80 + x) * 2;
}

}  // anonymous namespace.

namespace hal {

bool TextUI::initialized_ = false;

TextUI::TextUI() {}

bool TextUI::IsInitialized() {
  return initialized_;
}

void TextUI::Initialize() {
  // Disable the console blink. This also means we can use up to 16 colors.
  // In addition, console blinking is super annoying.
  inb(0x3DA);
  outb(0x3C0, 0x30);
  inb(0x3C1);
  outb(0x3C0, 0x30);

  // Clear the string, defaulting to gray on black.
  const uint8 formatting = (uint8(Color::Black) << 4) | uint8(Color::Gray);
  for (int i = 0; i < 80 * 25; i++) {
    screen_buffer[i * 2] = ' ';
    screen_buffer[i * 2 + 1] = formatting;
  }

  SetCursor(0, 0);
}

void TextUI::SetCursor(uint8 x, uint8 y) {
  size index = PosToIndex(x, y);
  uint16 pos = uint16(index / 2);
  const uint16 kCommandPort = 0x3D4;
  const uint16 kDataPort = 0x3D5;
  const uint16 kHighByteCommand = 14;
  const uint16 kLowByteCommand = 15;
  outb(kCommandPort, kHighByteCommand);
  outb(kDataPort, ((pos >> 8) & 0x00FF));
  outb(kCommandPort, kLowByteCommand);
  outb(kDataPort,    pos & 0x00FF);
}

void TextUI::SetChar(uint8 x, uint8 y, char c) {
  size index = PosToIndex(x, y);
  screen_buffer[index] = c;
}

void TextUI::SetColor(uint8 x, uint8 y, Color foreground, Color background) {
  size idx = PosToIndex(x, y);
  uint16 index = uint16(idx);
  uint8 foreground8 = uint8(foreground);
  uint8 background8 = uint8(background);
  screen_buffer[index + 1] = ((background8 & 0x0F) << 4) | (foreground8 & 0x0F);
}

void TextUI::Print(const char* msg, uint8 x, uint8 y) {
  TextUIOutputFn output_fn(x, y);
  (static_cast<klib::IOutputFn*>(&output_fn))->Print(msg);
}

TextUIOutputFn::TextUIOutputFn(uint8 x, uint8 y) :
  offset_x_(x), offset_y_(y) {
  // TODO(chris): CHECK valid.
}

void TextUIOutputFn::Print(char c) {
  TextUI::SetChar(OffsetX(), OffsetY(), c);
  offset_x_++;
  if (offset_x_ >= 80) {
    offset_x_ = 0;
    offset_y_++;
  }
  if (offset_y_ >= 25) {
    // WARNING: Overwriting last line. No scrolling.
    offset_y_ = 24;
  }
}

uint8 TextUIOutputFn::OffsetX() const { return offset_x_; }
uint8 TextUIOutputFn::OffsetY() const { return offset_y_; }

}  // namespace hal
