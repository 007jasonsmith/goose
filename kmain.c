#include "klib/console.h"
#include "klib/debug.h"
#include "sys/gdt.h"
#include "sys/idt.h"
#include "sys/isr.h"

// Expriments.
void kmain_crash();
void kmain_collatz_conjector();

const char version[] = "v0.1a";
void kmain(void) {
  debug_log("Kernel started.");

  // Initialize core CPU-based systems.
  gdt_install();  // Global descriptor table.
  idt_install();  // Interrupt descriptor table.
  isr_install();  // Interrupt servicer routines.

  con_initialize();
  con_write(HEADER_WIN, "Goose %s", version);

  con_writeline(OUTPUT_WIN, "World's most basic I/O program.");
  con_write(OUTPUT_WIN, "Press a character: ");
  con_win_readkey(OUTPUT_WIN);
  con_writeline(OUTPUT_WIN, "<newline>");
  con_writeline(OUTPUT_WIN, "Thanks!");

  // Crash, yo!
  //int denum = 1;
  //int result = 0 / (denum - 1);
  //con_writeline(OUTPUT_WIN, "Result was %d", result);

  debug_log("Kernel halted.");
}
#if 0
void kmain_crash() {
  fb_println("Goose %s - %s", version, "divide by zero crash ed.");

  // Division by Zero
  int denum = 1;
  int result = 0 / (denum - 1);
  fb_println("Result was %d", result);
}

void kmain_collatz_conjector() {
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
    }
  }
}
#endif
