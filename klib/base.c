#include "klib/base.h"

#include "klib/types.h"
#include "klib/strings.h"

// TODO(chris): Rename to base_.
void print_va(const char* msg, va_list args, OutputFn fn);
void print_str(const char* msg, OutputFn fn);
void print_int(int x, OutputFn fn);
void print_hex(int x, OutputFn fn);
void print_bin(int x, OutputFn fn);

// TODO(chris): Conform to some standard. (Ideally Go's printf, not libc's.)
void base_printf(const char* msg, OutputFn fn, ...) {
  va_list args;
  va_start(args, fn);
  print_va(msg, args, fn);
  va_end(args);
}

void print_va(const char* msg, va_list args, OutputFn fn) {
  size len = str_length(msg);
  for (size i = 0; i < len; i++) {
    char c = msg[i];

    if (c != '%') {
      fn(c);
      continue;
    }

    // Was the percent escaped?
    if (i > 0 && msg[i - 1] == '\\') {
      fn('%');
      continue;
    }

    // Get the specifier.
    if (i == len - 1) {
      // TODO(chris): Panic, ending with '%'.
      fn('%');
      continue;
    }
    c = msg[i + 1];

    // TODO(chris): Put these into a union.
    int int_arg;
    uint32 uint_arg;
    char* char_ptr;
    uint32 ptr_address;

    switch (c) {
    case 'd':
      int_arg = va_arg(args, int);
      if (int_arg < 0) {
        fn('-');
        int_arg *= -1;
      }
      print_int(int_arg, fn);
      break;
    case 's':
      char_ptr = va_arg(args, char*);
      print_str(char_ptr, fn);
      break;
    case 'b':
      uint_arg = va_arg(args, uint32);
      print_str("0b", fn);
      print_bin(uint_arg, fn);
      break;
    case 'p':
    case 'h':  // pointers and hex behave the same.
      ptr_address = va_arg(args, uint32);
      print_str("0x", fn);
      print_hex(ptr_address, fn);
      break;
    default:
      // TODO(chris): Panic.
      // debug_log("Error: Unknown format specifier.");
      ;
    }
    // Skip the format specifier.
    i++;
 }
}

void print_str(const char* msg, OutputFn fn) {
  while(*msg) {
    fn(*msg);
    msg++;
  }
}

void print_int(int x, OutputFn fn) {
  int digit = x % 10;
  if (x >= 10) {
    print_int(x / 10, fn);
  }
  fn('0' + digit);
}

const char kHexDigits[] = "0123456789ABCDEF";
void print_hex(int x, OutputFn fn) {
  int digit_idx = x % 16;
  if (x >= 16) {
    print_hex(x / 16, fn);
  }
  fn(kHexDigits[digit_idx]);
}

void print_bin(int x, OutputFn fn) {
  int bit = x % 2;
  if (x >= 2) {
    print_bin(x / 2, fn);
  }
  fn(bit ? '1' : '0');
}
