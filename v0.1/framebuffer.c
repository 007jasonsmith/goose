#include "framebuffer.h"

#include "io.h"

// TODO(chris): Figure out where this comes from... Does
// the monitor memory map its I/O to this address automatically?
// Or is it set in our GRUB script?
char* const fb = (char*) 0x000B8000;

void fb_clear(void) {
  // 80 x 25 cells at 2 bytes each.
  const char color = (FB_GRAY & 0x0F);  // Grey on black.
  for (int i = 0; i < 80 * 25 * 2; i += 2) {
    fb[i] = ' ';
    fb[i+1] = color;
  }
}

void fb_write_cell(uint16_t i, char c, FbColor foreground, FbColor background) {
    fb[i * 2] = c;
    fb[i * 2 + 1] = ((background & 0x0F) << 4) | (foreground & 0x0F);
}

// Constants used to interact with the VGA registers.
const uint16_t kCommandPort = 0x3D4;
const uint16_t kDataPort = 0x3D5;
const uint16_t kHighByteCommand = 14;
const uint16_t kLowByteCommand = 15;


void fb_move_cursor(uint16_t pos) {
  outb(kCommandPort, kHighByteCommand);
  outb(kDataPort, ((pos >> 8) & 0x00FF));
  outb(kCommandPort, kLowByteCommand);
  outb(kDataPort,    pos & 0x00FF);
}

void fb_disable_blink(void) {
  inb(0x3DA);
  outb(0x3C0, 0x30);
  inb(0x3C1);
  outb(0x3C0, 0x30);

}
