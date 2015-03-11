#include "klib/base.h"

#include "klib/types.h"
#include "klib/strings.h"

// TODO(chris): Rename to base_.
void print_str(const char* msg, OutputFn fn);
void print_int(int x, OutputFn fn);
void print_uint(uint32 x, OutputFn fn);
void print_hex(int x, OutputFn fn);
void print_bin(int x, OutputFn fn);

// TODO(chris): Conform to some standard. (Ideally Go's printf, not libc's.)
void base_printf(const char* msg, OutputFn fn, ...) {
  vararg_list args;
  vararg_start(args, fn);
  base_printf_va(msg, args, fn);
  vararg_end(args);
}

void base_printf_va(const char* msg, vararg_list args, OutputFn fn) {
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

    char char_arg;

    switch (c) {
    case 'c':
      char_arg = vararg_arg(args, char);
      fn('[');
      fn(char_arg);
      fn(']');
      fn('[');
      fn(char_arg & 128 ? '1' : '0');
      fn(char_arg & 64 ? '1' : '0');
      fn(char_arg & 32 ? '1' : '0');
      fn(char_arg & 16 ? '1' : '0');
      fn(char_arg & 8 ? '1' : '0');
      fn(char_arg & 4 ? '1' : '0');
      fn(char_arg & 2 ? '1' : '0');
      fn(char_arg & 1 ? '1' : '0');
      fn(']');
      break;
      /*
    case 'u':
      uint_arg = vararg_arg(args, uint32);
      print_uint(uint_arg, fn);
      break;
    case 'd':
      int_arg = vararg_arg(args, int32);
      if (int_arg < 0) {
        fn('-');
        int_arg *= -1;
      }
      print_int(int_arg, fn);
      break;
    case 's':
      char_ptr = vararg_arg(args, char*);
      print_str(char_ptr, fn);
      break;
    case 'b':
      uint_arg = vararg_arg(args, uint32);
      print_str("0b", fn);
      print_bin(uint_arg, fn);
      break;
    case 'p':
    case 'h':  // pointers and hex behave the same.
      ptr_address = vararg_arg(args, uint32);
      print_str("0x", fn);
      print_hex(ptr_address, fn);
      break;
      */
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

void print_uint(uint32 x, OutputFn fn) {
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
