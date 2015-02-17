#include "lib/debug.h"
#include "lib/framebuffer.h"
#include "lib/string.h"
#include "lib/types.h"

#include "sys/gdt.h"

void hack_print(int n);
void hack_sleep();

// EXPERIMENTAL.
typedef struct  __attribute__((packed)) {
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
  // TODO(chris): What other registers are important to persist?
  unsigned int esp;
} CpuState;


const char version[] = "v0.1a";
void kmain(void) {
  debug_log("Kernel %s loaded successfully.", version);
  debug_log("sizeof(int8_t)  = %d", sizeof(int8_t));
  debug_log("sizeof(int16_t) = %d", sizeof(int16_t));
  debug_log("sizeof(int32_t) = %d", sizeof(int32_t));
  debug_log("sizeof(size_t)   = %d", sizeof(size_t));
  debug_log("sizeof(void*) = %d", sizeof(void*));

  // Install a new global descriptor table.
  // TODO(chrsmith): Are the three, uh, descriptors, enough?
  gdt_install();

  fb_clear();
  fb_println("Experiment finished. Inspect COM1 for results.");
}

void kmain_collatz_conjector() {
  // Set up the VGA text mode.
  fb_clear();
  fb_disable_blink();
  // TODO(chris): Disable the cursor.

  fb_println("Goose %s - %s", version, "Collatz conjecture ed");
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
