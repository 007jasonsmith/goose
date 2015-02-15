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

  // TODO(chris): Implement printf.
  fb_println("Goose v0.1a - Collatz conjecture ed");
  for (int i = 2; i < 9; i++) {
    char header[] = "===== Processing ? =====";
    header[17] = '0' + i;
    fb_println(header);
    int n = i;
    while (n != 1) {
      hack_print(n);
      if (n % 2 == 1) {
	n = n * 3 + 1;
      } else {
	n /= 2;
      }

      hack_sleep();
    }
  }
}

// Workaround until printf exists.
void hack_print(int n) {
  char buffer[] = "        ";
  int idx = strlen(buffer) - 1;
  while (n > 0) {
    buffer[idx] = '0' + (n % 10);
    n /= 10;
    idx--;
  }
  fb_println(buffer);
}

// Workaround not having any timing routines.
void hack_sleep() {
  for (int i = 0; i < 100000; i++) {
    fb_move_cursor(0);
  }
}
