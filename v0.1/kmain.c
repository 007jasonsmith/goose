#include "debug.h"
#include "framebuffer.h"

void write(const char* const buf, int line, int len);

void kmain(void) {
  fb_disable_blink();
  fb_clear();

  write("Goose v0.1a", 0, 11);
  debug_log("Goose booted up!\n");

  char hex_lookup[] = "0123456789ABCDEF";
  int line = 1;
  for (FbColor f = 0; f < 16; f++) {
    for (FbColor b = 0; b < 16; b++) {
      fb_write_cell(b * 2 + line * 80, hex_lookup[(int) f], f, b);
      fb_write_cell(b * 2 + 1 + line * 80, hex_lookup[(int) b], f, b);
    }
    line++;
  }

  while (1) {
    for (int16_t i = 0; i < 80 * 25; i++) {
      // Artifically slow thigns down since LOL no time().
      for (int j = 0; j < 50000; j++) {
	fb_move_cursor(i);
      }
    }
  }
}

void write(const char* const buf, int line, int len) {
  for (int i = 0; i < len; i++) {
    fb_write_cell(i + line * 80, buf[i], FB_RED, FB_WHITE);
  }
}
