#include "klib/console.h"
#include "klib/debug.h"
#include "klib/strings.h"
#include "sys/gdt.h"
#include "sys/idt.h"
#include "sys/isr.h"

// Expriments.
void kmain_crash();
void kmain_collatz_conjector();

extern "C" {

const char version[] = "v0.1a";
void kmain(void) {
  debug_log("Kernel started.");

  // Initialize core CPU-based systems.
  gdt_install();  // Global descriptor table.
  idt_install();  // Interrupt descriptor table.
  isr_install();  // Interrupt servicer routines.

  con_initialize();
  con_write(HEADER_WIN, "Goose %s", version);

  con_writeline(OUTPUT_WIN, "Keyboard echo.");

  char command[256];
  while (true) {
    con_write(OUTPUT_WIN, "> ");
    con_win_readline(OUTPUT_WIN, command, 256);
    con_writeline(OUTPUT_WIN, "Executing '%s'", command);

    if (str_compare(command, "crash")) {
      int denum = 1;
      int result = 0 / (denum - 1);
      con_writeline(OUTPUT_WIN, "Result was %d", result);
    }
    if (str_compare(command, "exit")) {
      break;
    }
  }

  debug_log("Kernel halted.");
}

}
