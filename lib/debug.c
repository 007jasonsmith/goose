#include "lib/debug.h"

#include "lib/types.h"
#include "lib/printf.h"
#include "sys/io.h"

/**
 * Wrap COM1 for debugging purposes.
 * See: http://wiki.osdev.org/Serial_Ports
 */

const int COM1 = 0x3f8;

void initialize(void);
bool is_transmit_empty(void);
void write_char(char c);

void debug_log(const char* const buf, ...) {
  static bool initialized = false;
  if (!initialized) {
    initialize();
    initialized = true;
  }

  va_list args;
  va_start(args, buf);

  base_printf(buf, args, &write_char);
  write_char('\n');
}

// TODO(chrsmith): This needs to be refactored to include leading
// zeros. Things sould be aligned.
void debug_log_obj(const char* name, void* obj, size_t bytes) {
  OutputFn fn = &write_char;
  uint32_t ptr_address = (uint32_t) obj;
  print_va("Dump of object '%s' located at %p[%d bytes]\n", fn, name, obj, bytes);
  if (ptr_address % 4 != 0 || bytes % 4 != 0) {
    print_str("WARNING: Object is not aligned.", fn);
  }
  if (obj == null) {
    print_str("ERROR: null", &write_char);
  }

  int* obj2 = (int*) obj;
  for (size_t i = 0; i < bytes / (size_t) sizeof(void*); i++) {
    ptr_address = (uint32_t) obj2[i];
    print_va("%d: %h %d %b\n", fn, i, ptr_address, ptr_address, ptr_address);
  }
}

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

void write_char(char c) {
  while (!is_transmit_empty()) {
    // Wait until the buffer is clear.
  }

  outb(COM1, (uint32_t) c);
}
