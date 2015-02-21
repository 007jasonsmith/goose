#include "klib/debug.h"

#include "klib/base.h"
#include "sys/io.h"
#include "klib/types.h"

// Wrapping COM1 for debugging purposes.
// http://wiki.osdev.org/Serial_Ports

const uint32 COM1 = 0x03F8;

// Private methods
void debug_com_intialize();
bool debug_com_is_transmit_empty();
void debug_com_write_char(char c);

void debug_log(const char* msg, ...) {
  va_list args;
  va_start(args, msg);
  base_printf_va(msg, args, debug_com_write_char);
  va_end(args);

  debug_com_write_char('\n');
}

void debug_com_initialize() {
  outb(COM1 + 1, 0x00);  // Disable all interrupts
  outb(COM1 + 3, 0x80);  // Enable DLAB (set baud rate divisor)
  outb(COM1 + 0, 0x03);  // Set divisor to 3 (lo byte) 38400 baud
  outb(COM1 + 1, 0x00);  //                  (hi byte)
  outb(COM1 + 3, 0x03);  // 8 bits, no parity, one stop bit
  outb(COM1 + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
  outb(COM1 + 4, 0x0B);  // IRQs enabled, RTS/DSR set
}

bool debug_com_is_transmit_empty() {
  return ((inb(COM1 + 5) & 0x20) != 0);
}

void debug_com_write_char(char c) {
  static bool initialized = false;
  if (!initialized) {
    debug_com_initialize();
    initialized = true;
  }

  while (!debug_com_is_transmit_empty()) {
    // Wait until the buffer is clear.
  }
  outb(COM1, (uint32_t) c);
}
