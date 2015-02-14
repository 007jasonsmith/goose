#include "debug.h"

#include "io.h"
#include "fixed_types.h"

/**
 * Wrap COM1 for debugging purposes.
 * See: http://wiki.osdev.org/Serial_Ports
 */

const int COM1 = 0x3f8;

void initialize(void) {
  outb(COM1 + 1, 0x00);    // Disable all interrupts
  outb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outb(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  outb(COM1 + 1, 0x00);    //                  (hi byte)
  outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
  outb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

bool is_transmit_empty(void) {
  return ((inb(COM1 + 5) & 0x20) != 0);
}

void debug_log(const char* const buf) {
  static bool initialized = false;
  if (!initialized) {
    initialize();
    initialized = true;
  }

  int idx = 0;
  while (buf[idx]) {
    while (!is_transmit_empty()) {
      // Wait until the buffer is clear.
    }

    outb(COM1, (uint32_t) buf[idx]);
    idx++;
  }
}
