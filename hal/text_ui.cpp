#include "hal/text_ui.h"

#include "klib/types.h"
#include "sys/io.h"
#include "klib/panic.h"
// See: http://wiki.osdev.org/Text_UI

namespace {

// NOTE: The actual address is 0x000B8000. However, we virtual memory to map
// 0x00000000 to 0xC0000000.
uint8* const screen_buffer = (uint8*) 0xC00B8000;

// Returns the raw index into the screen_buffer. So screen_buffer[index] is the
// character, and screen_buffer[index + 1] is the formatting.
size PosToIndex(uint8 x, uint8 y) {
  return (y * 80 + x) * 2;
}

}  // anonymous namespace.

namespace hal {

bool TextUI::initialized_ = false;
bool TextUI::show_cursor_ = true;
Offset TextUI::cursor_;

Offset::Offset() : x(0), y(0) {}

Offset::Offset(uint8 x, uint8 y) : x(x), y(y) {}

Region::Region(uint8 x, uint8 y, uint8 width, uint8 height) {
  this->offset.x = x;
  this->offset.y = y;
  this->width = width;
  this->height = height;
}

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
  // TODO(chris): This is an ugly hack. Disable the cursor by simply
  // setting the display cursor flag in the VGA settings register.
  cursor_.x = x;
  cursor_.y = y;
  if (!show_cursor_) {
    return;
  }

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

void TextUI::ShowCursor(bool show) {
  if (show) {
    SetCursor(cursor_.x, cursor_.y);
  } else {
    SetCursor(81, 26);  // Off the screen.
  }
  show_cursor_ = show;
}

void TextUI::Scroll(const Region& region) {
  if (region.height < 2) {
    klib::Panic("region.height < 2?");
    return;
  }
  for (int y = region.offset.y; y < region.offset.y + region.height - 1; y++) {
    for (int x = region.offset.x; x < region.offset.x + region.width; x++) {
      size index = PosToIndex(x, y);
      screen_buffer[index] = screen_buffer[index + 80*2];
      screen_buffer[index + 1] = screen_buffer[index + 80*2 + 1];
    }
  }

  int y = region.offset.y + region.height - 1;
  for (int x = region.offset.x; x < region.offset.x + region.width; x++) {
      size index = PosToIndex(x, y);
      screen_buffer[index] = ' ';
  }
}

TextUIOutputFn::TextUIOutputFn(uint8 x, uint8 y) {
  // TODO(chris): CHECK valid.
  offset_.x = x;
  offset_.y = y;
}

void TextUIOutputFn::Print(char c) {
  // Don't scroll.
  if (offset_.x >= 80) {
    return;
  }
  TextUI::SetChar(offset_.x, offset_.y, c);
  offset_.x++;
}

}  // namespace hal
