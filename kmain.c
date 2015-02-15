#include "lib/debug.h"
#include "lib/framebuffer.h"
#include "lib/string.h"

void hack_print(int n);
void hack_sleep();

void kmain(void) {
  debug_log("Kernel loaded successfully.");

  // Set up the VGA text mode.
  fb_clear();
  fb_disable_blink();
  // TODO(chris): Disable the cursor.

  fb_println("Goose v0.1a - Collatz conjecture ed");
  for (int i = 2; i < 9; i++) {
    fb_println("===== Processing %d =====", i);
    int n = i;
    while (n != 1) {
      fb_println("    %d", n);
      if (n % 2 == 1) {
	n = n * 3 + 1;
      } else {
	n /= 2;
      }

      hack_sleep();
    }
  }
}

// Workaround not having any timing routines.
void hack_sleep() {
  for (int i = 0; i < 100000; i++) {
    fb_move_cursor(0);
  }
}
